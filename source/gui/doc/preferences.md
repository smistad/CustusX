Preferences
===================


\addindex preferences_general_widget
General {#preferences_general_widget}
===
General preferences

- Patient data folder: All patients folders will be stored here.
- Profile: All settings and configurations will be stored in this profile. Press the "+" button to copy the current one and create a new profile.
- VLC path: Path to the VLC media player. Used for recording videos of the application.
- Tool smoothing: Smooth positions of tracked tools.


\addindex preferences_performance_widget
Performance {#preferences_performance_widget}
===
Parameters affecting the performance of the application.

- Interval between renderings
- Max Render Size: Volumes are downsampled to this size before rendered in 3D (2D still shows slices through original data).
- Smart Render: Render only once per second if there is no changes in the scene.
- 2D Overlay: Enable multiple volumes in 2D (GPU based multi slicer, can only handle up to 4 images).
- Optimized views: Speedup by merging all 2D views into a single vtkRenderWindow
- Still Update Rate: Property in vtkRenderWindow. Increasing this value may improve rendering speed at the cost of quality.
- 3D Renderer: Select method for rendering images in 3D


\addindex preferences_automation_widget
Automation {#preferences_automation_widget}
===
Turn on/off automated operations.


\addindex preferences_visualization_widget
Visualization {#preferences_visualization_widget}
===
Preferences for 2D and 3D visualization.


\addindex preferences_video_widget
Video {#preferences_video_widget}
===
Preferences for aqcuired video (with video grabber or digital interface).

- Choose a prefix for the grabbed video.
- Save video in either 24 bit color or 8 bit greyscale (Digital video already in 8 bit will not be converted to 24 bit).
- Compress stored video.


\addindex preferences_tool_config_widget
Tool Configuration {#preferences_tool_config_widget}
===
Setup tool configuration for tracking


\addindex preferences_debug_widget
Debug {#preferences_debug_widget}
===
Preferences used for debugging.
