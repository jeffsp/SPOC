import matplotlib as mpl
import matplotlib.pyplot as plt

with open('class_palette.txt', 'rt') as pal_file:
    lines = pal_file.readlines()

hex_colors = []
for line in lines:
    hex_colors.append('#' + line.strip().strip('R"(').strip(')"'))

fig, ax = plt.subplots(figsize=(6, 1))
fig.subplots_adjust(bottom=0.5)

# Create a LinearSegmentedColormap with the hex colors
cmap = mpl.colors.LinearSegmentedColormap.from_list("", hex_colors)

# Set boundaries for discrete color segments
ticks = range(len(hex_colors))  # tick marks
bounds = [] # Boundries for each color
for tick in ticks:
    bounds.append(tick - 0.5)
norm = mpl.colors.BoundaryNorm(bounds, cmap.N)

cb2 = mpl.colorbar.ColorbarBase(
   ax,
   cmap=cmap,
   norm=norm,
   boundaries=bounds,  # Use the same boundaries for ticks
   extend='neither',  # No extension for discrete colors
   ticks=ticks,
   spacing='uniform',  # Uniform spacing for discrete colors
   orientation='horizontal'
)
cb2.set_label('Class color reference')
fig.show()
input('Press any key to close...')
