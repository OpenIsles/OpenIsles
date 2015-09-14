import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100

bpy.context.scene.render.filepath = "render/minimap.png"
bpy.ops.render.render(write_still = True)