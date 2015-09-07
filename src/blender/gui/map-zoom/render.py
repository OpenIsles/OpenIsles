import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100

bpy.context.scene.render.filepath = "render/map-zoom.png"
bpy.ops.render.render(write_still = True)

bpy.data.objects["Plus-Highlight-Spot"].hide_render = False
bpy.context.scene.render.filepath = "render/map-zoom-highlight-plus.png"
bpy.ops.render.render(write_still = True)

bpy.data.objects["Minus-Highlight-Spot"].hide_render = False
bpy.context.scene.render.filepath = "render/map-zoom-highlight-minus.png"
bpy.ops.render.render(write_still = True)