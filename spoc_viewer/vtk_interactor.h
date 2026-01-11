#ifndef VTK_INTERACTOR_H
#define VTK_INTERACTOR_H

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCellArray.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLight.h>
#include <vtkLightCollection.h>
#include <vtkObjectFactory.h>
#include <vtkPNGWriter.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolygon.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkUnsignedCharArray.h>
#include <vtkVertexGlyphFilter.h>
#include <vtkWindowToImageFilter.h>
#include <vtkXOpenGLRenderWindow.h>
#include <vtkXRenderWindowInteractor.h>
#include <sstream>

#include "palette.h"

namespace spoc_viewer
{

namespace vtk_interactor
{

// A file for sharing camera coordinates between applications
const std::string camera_coordinates_filename ("/tmp/spoc_viewer_camera_coordinates.txt");

template<typename T>
void set_camera_coordinates (T camera, const std::string &camera_coordinates)
{
    // Parse the camera_coordinates string:
    //
    // First remove comma separators
    std::string c (camera_coordinates);
    std::replace (c.begin (), c.end (), ',', ' ');
    // The string should contain 3 sets of x,y,z points
    double x, y, z;
    std::stringstream ss (c);
    ss >> x >> y >> z;
    std::clog << "Read position " << x << ',' << y << ',' << z << std::endl;
    camera->SetPosition (x, y, z);
    ss >> x >> y >> z;
    std::clog << "Read focalpoint " << x << ',' << y << ',' << z << std::endl;
    camera->SetFocalPoint (x, y, z);
    ss >> x >> y >> z;
    std::clog << "Read viewup " << x << ',' << y << ',' << z << std::endl;
    camera->SetViewUp (x, y, z);
}

// Customization of an InteractorStyle
class CustomInteractorStyle : public vtkInteractorStyleTrackballCamera
{
    public:
    static CustomInteractorStyle* New();
    vtkTypeMacro(CustomInteractorStyle, vtkInteractorStyleTrackballCamera);

    CustomInteractorStyle ()
    {
        textActor = vtkSmartPointer<vtkTextActor>::New();
        SetHelp (DEFAULT_TEXT);
    }
    void SetHelp (const char *text)
    {
        textActor->SetInput (text);
        textActor->SetPosition2 (10, 40);
        textActor->GetTextProperty ()->SetFontSize (24);
        textActor->GetTextProperty ()->SetColor (1.0, 1.0, 1.0);
    }
    void OnChar()
    {
        // Get the keypress
        vtkRenderWindowInteractor *rwi = this->Interactor;
        std::string key = rwi->GetKeySym();

        if (key == "Escape")
            rwi->TerminateApp ();
        else if (key == "Up" || key == "KP_Add")
            OnMouseWheelForward ();
        else if (key == "Down" || key == "KP_Subtract")
            OnMouseWheelBackward ();
        // "Vertical align" command
        else if (key == "space")
        {
            auto renderer = GetCurrentRenderer ();
            auto camera = renderer->GetActiveCamera ();
            camera->SetViewUp (0, 0, 1);
            GetInteractor ()->Render ();
        }
        // Dump camera coordinates
        else if (key == "c")
        {
            auto renderer = GetCurrentRenderer ();
            auto camera = renderer->GetActiveCamera ();
            double px, py, pz;
            double fx, fy, fz;
            double vx, vy, vz;
            camera->GetPosition (px, py, pz);
            camera->GetFocalPoint (fx, fy, fz);
            camera->GetViewUp (vx, vy, vz);
            // Write them to the terminal
            std::clog << std::fixed;
            std::clog << px << ',' << py << ',' << pz << ',';
            std::clog << fx << ',' << fy << ',' << fz << ',';
            std::clog << vx << ',' << vy << ',' << vz << std::endl;
            // Also write them to a file
            std::clog << "Writing camera coordinates to " << camera_coordinates_filename << std::endl;
            std::ofstream ofs (camera_coordinates_filename);
            if (!ofs)
            {
                std::clog << "Can't open file for writing" << std::endl;
            }
            else
            {
                ofs << std::fixed;
                ofs << px << ' ' << py << ' ' << pz << ' ';
                ofs << fx << ' ' << fy << ' ' << fz << ' ';
                ofs << vx << ' ' << vy << ' ' << vz << std::endl;
            }

            GetInteractor ()->Render ();
        }
        // Read camera coordinates
        else if (key == "a")
        {
            auto renderer = GetCurrentRenderer ();
            auto camera = renderer->GetActiveCamera ();
            std::clog << "Reading camera coordinates from " << camera_coordinates_filename << std::endl;
            std::ifstream ifs (camera_coordinates_filename);
            if (!ifs)
            {
                std::clog << "Can't open file for reading" << std::endl;
            }
            else
            {
                double x, y, z;
                ifs >> x >> y >> z;
                std::clog << "Read position " << x << ',' << y << ',' << z << std::endl;
                camera->SetPosition (x, y, z);
                ifs >> x >> y >> z;
                std::clog << "Read focalpoint " << x << ',' << y << ',' << z << std::endl;
                camera->SetFocalPoint (x, y, z);
                ifs >> x >> y >> z;
                std::clog << "Read viewup " << x << ',' << y << ',' << z << std::endl;
                camera->SetViewUp (x, y, z);
            }

            // Set the clipping range to include all points
            renderer->ResetCameraClippingRange ();
            GetInteractor ()->Render ();
        }
        // Help text
        else if (key == "h" || key == "question")
        {
            help_on = !help_on;
            if (help_on)
                SetHelp (HELP_TEXT);
            else
                SetHelp (DEFAULT_TEXT);
            GetInteractor ()->Render ();
        }

        // Forward all other events
        vtkInteractorStyleTrackballCamera::OnChar();
    }
    vtkSmartPointer<vtkTextActor> textActor;
    private:
    bool help_on = false;
    static constexpr const char *DEFAULT_TEXT = "Help=?|H";
    static constexpr const char *HELP_TEXT =
        "Q,Esc=Quit | +-=Zoom | C=Write-camera-coords | A=Read-camera-coords | F=Fly-to | Space=Vertical-align | H=Help";
};

// Required for the object factories
vtkStandardNewMacro(CustomInteractorStyle);

template<typename P,typename C>
void start_interactor (const P pc,
        const C &rgbs,
        const std::string &camera_coordinates,
        const std::string &fn,
        const double resolution,
        const bool box_mode,
        const float opacity,
        const size_t point_size)
{
    // There should be one RGB per point
    assert (pc.size () == rgbs.size ());

    const auto e = spoc::extent::get_extent (pc);

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    for (auto p : pc)
        points->InsertNextPoint (p.x - e.minp.x, p.y - e.minp.y, p.z - e.minp.z);

    vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3);
    colors->SetName ("Colors");

    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);

    vtkSmartPointer<vtkVertexGlyphFilter> vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    vertexFilter->AddInputData(polydata);
    vertexFilter->Update();

    // Set the RGBs
    for (auto c : rgbs)
    {
        const unsigned char color[3] = {
            static_cast<unsigned char> (c[0]),
            static_cast<unsigned char> (c[1]),
            static_cast<unsigned char> (c[2])};
        colors->InsertNextTypedTuple(color);
    };

    polydata->GetPointData()->SetScalars(colors);

    // Visualization
    vtkSmartPointer<vtkPolyDataMapper> pcMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> pcActor = vtkSmartPointer<vtkActor>::New();
    pcActor->SetMapper(pcMapper);
    pcActor->GetProperty()->SetOpacity(opacity);

    // Add the point cloud points
    if (box_mode)
    {
        vtkSmartPointer<vtkCubeSource> cubeSource = vtkSmartPointer<vtkCubeSource>::New();
        vtkSmartPointer<vtkGlyph3D> glyph3D = vtkSmartPointer<vtkGlyph3D>::New();
        glyph3D->SetColorModeToColorByScalar();
        glyph3D->SetSourceConnection(cubeSource->GetOutputPort());
        glyph3D->SetInputData(polydata);
        glyph3D->ScalingOff();
        glyph3D->Update();
        cubeSource->SetXLength(resolution);
        cubeSource->SetYLength(resolution);
        cubeSource->SetZLength(resolution);
        cubeSource->SetCenter(0.0,0.0,0.0);
        pcMapper->SetInputConnection(glyph3D->GetOutputPort());
    }
    else
    {
        pcActor->GetProperty()->SetPointSize(point_size);
        pcMapper->SetInputConnection(vertexFilter->GetOutputPort());
    }

    // Create the renderer and add the point cloud points
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);
    renderWindow->SetSize(1920,1080);
    renderWindow->SetWindowName(fn.c_str());
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderer->AddActor(pcActor);

    // Set the style
    renderWindowInteractor->SetRenderWindow(renderWindow);
    vtkSmartPointer<vtk_interactor::CustomInteractorStyle> style = vtkSmartPointer<vtk_interactor::CustomInteractorStyle>::New();
    style->SetCurrentRenderer(renderer);
    renderWindowInteractor->SetInteractorStyle (style);

    // Set background
    renderer->SetBackground (0, 0, 0);

    // Position Camera Angle
    auto camera = renderer->GetActiveCamera ();
    if (camera_coordinates.empty ())
    {
        camera->SetPosition (0, -0.75, 0.5);
        camera->SetFocalPoint (0, 0, 0);

        // Move along current view axis until all points are in the viewport
        renderer->ResetCamera ();

        // Zoom in closer
        style->OnMouseWheelForward();
        style->OnMouseWheelForward();
        style->OnMouseWheelForward();
    }
    else
    {
        set_camera_coordinates (camera, camera_coordinates);

        // Set the clipping range to include all points
        renderer->ResetCameraClippingRange ();
    }

    // Display in perspective mode
    renderer->GetActiveCamera()->ParallelProjectionOff();

    // Add lights
    renderer->GetLights()->RemoveAllItems ();
    vtkSmartPointer<vtkLight> light1 = vtkSmartPointer<vtkLight>::New();
    vtkSmartPointer<vtkLight> light2 = vtkSmartPointer<vtkLight>::New();
    light1->SetPosition(-1, -2, 1);
    light2->SetPosition( 1,  1.5, 2);
    light1->SetLightTypeToSceneLight();
    light2->SetLightTypeToSceneLight();
    renderer->AddLight(light1);
    renderer->AddLight(light2);
    renderWindow->Render();

    // Show text
    renderer->AddActor2D (style->textActor);

    // Enter event loop
    renderWindowInteractor->Start ();
}

} // namespace vtk_interactor

} // namespace spoc_viewer

#endif // VTK_INTERACTOR_H
