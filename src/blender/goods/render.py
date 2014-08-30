import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100
bpy.data.scenes["Scene"].render.resolution_x = 40
bpy.data.scenes["Scene"].render.resolution_y = 40
bpy.context.scene.render.filepath = "marketplace-icon.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].render.resolution_x = 32
bpy.data.scenes["Scene"].render.resolution_y = 32
bpy.context.scene.render.filepath = "icon.png"
bpy.ops.render.render(write_still = True)
