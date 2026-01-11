#pragma once

#include <archive.h>
#include <archive_entry.h>
#include <iostream>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace spoc
{

namespace web_dataset
{

class WebDSWriter
{
    private:
    struct archive* a;
    std::ostream &os;

    static ssize_t write_callback (struct archive *, void *client_data, const void *buffer, size_t length)
    {
        auto *self = static_cast<WebDSWriter*> (client_data);
        self->os.write (static_cast<const char*>(buffer), length);
        return self->os ? length // Success
            : -1; // Error
    }

    public:
    explicit WebDSWriter (std::ostream& output_stream)
        : a (archive_write_new ())
        , os (output_stream)
    {
        archive_write_set_format_pax_restricted (a); // Enable TAR
        archive_write_add_filter_gzip (a); // Enable GZIP

        // Pass this object as client data
        if (archive_write_open (a, this, nullptr, write_callback, nullptr) != ARCHIVE_OK)
            throw std::runtime_error("libarchive.archive_write_open() failed");
    }
    ~WebDSWriter()
    {
        // Cleanup
        archive_write_close (a);
        archive_write_free (a);
    }
    // Write a file with the given name/data to the archive
    void write (const std::string &name, std::span<const char> data)
    {
        struct EntryGuard
        {
            struct archive_entry *entry {};
            EntryGuard () : entry (archive_entry_new ()) { }
            ~EntryGuard () { archive_entry_free(entry); }
        } eg;

        archive_entry_set_pathname (eg.entry, name.c_str ());
        archive_entry_set_size (eg.entry, data.size ());
        archive_entry_set_filetype (eg.entry, AE_IFREG); // Regular file
        archive_entry_set_perm (eg.entry, 0644); // Owner R/W

        if (archive_write_header (a, eg.entry) != ARCHIVE_OK)
            throw std::runtime_error (archive_error_string (a));
        if (archive_write_data (a, data.data(), data.size()) < 0)
            throw std::runtime_error (archive_error_string (a));
    }
};

struct WebDSFile
{
    std::string name {};
    std::vector<char> data {};
};

class WebDSReader
{
    private:
    struct archive *a;
    std::istream &is;
    std::vector<char> buffer;
    static constexpr size_t block_size = 16 * 1024;

    static ssize_t read_callback (struct archive *, void *client_data, const void **buffer)
    {
        auto *self = static_cast<WebDSReader*> (client_data);

        // Read from stream into our internal buffer
        self->is.read (self->buffer.data(), self->buffer.size());
        size_t bytes_read = self->is.gcount ();

        if (bytes_read == 0 && self->is.eof())
            return 0; // Normal EOF
        if (self->is.bad())
            return -1; // I/O Error

        // Set the pointer to the data we just read
        *buffer = self->buffer.data();
        return bytes_read;
    }

    public:
    explicit WebDSReader (std::istream &input_stream)
        : a (archive_read_new())
        , is (input_stream)
        , buffer (block_size)
    {
        archive_read_support_filter_all (a); // Auto-detect gzip/bzip2
        archive_read_support_format_all (a); // Auto-detect tar

        if (archive_read_open (a, this, nullptr, read_callback, nullptr) != ARCHIVE_OK)
            throw std::runtime_error("libarchive.archive_read_open() failed");
    }
    ~WebDSReader()
    {
        // Cleanup
        archive_read_free(a);
    }
    // Get the next file in the archive
    std::optional<WebDSFile> next ()
    {
        struct archive_entry *entry;
        int r = archive_read_next_header (a, &entry);

        if (r == ARCHIVE_EOF)
            return std::nullopt; // End of archive
        if (r != ARCHIVE_OK)
            throw std::runtime_error (archive_error_string (a));

        WebDSFile file;
        file.name = archive_entry_pathname (entry);
        const size_t size = archive_entry_size (entry);
        file.data.resize (size);
        archive_read_data (a, file.data.data (), size);
        return file;
    }
};

} // namespace web_dataset

} // namespace spoc
