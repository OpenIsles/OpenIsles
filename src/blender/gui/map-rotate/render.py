import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100

angles = {'south': 225, 'east': 135, 'north': 45, 'west': 315}
for viewname, angle in angles.items():
    bpy.data.objects["Kompass-Nadel"].rotation_euler.y = angle / 180 * math.pi

    bpy.data.objects["Pfeil-Highlight-Spot links"].hide_render = True
    bpy.data.objects["Pfeil-Highlight-Spot rechts"].hide_render = True
    bpy.context.scene.render.filepath = "render/map-rotate-" + viewname + ".png"
    bpy.ops.render.render(write_still = True)

    bpy.data.objects["Pfeil-Highlight-Spot links"].hide_render = False
    bpy.data.objects["Pfeil-Highlight-Spot rechts"].hide_render = True
    bpy.context.scene.render.filepath = "render/map-rotate-" + viewname + "-highlight-left.png"
    bpy.ops.render.render(write_still = True)

    bpy.data.objects["Pfeil-Highlight-Spot links"].hide_render = True
    bpy.data.objects["Pfeil-Highlight-Spot rechts"].hide_render = False
    bpy.context.scene.render.filepath = "render/map-rotate-" + viewname + "-highlight-right.png"
    bpy.ops.render.render(write_still = True)