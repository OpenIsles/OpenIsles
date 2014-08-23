import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100
bpy.data.scenes["Scene"].render.resolution_x = 45
bpy.data.scenes["Scene"].render.resolution_x = 42
bpy.context.scene.render.filepath = "marketplace-icon.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].render.resolution_x = 32
bpy.data.scenes["Scene"].render.resolution_y = 32
bpy.data.objects["Hintergrund"].hide_render = True
bpy.context.scene.render.filepath = "icon.png"
bpy.ops.render.render(write_still = True)
