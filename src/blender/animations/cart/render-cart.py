import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100

bpy.data.objects["Ladung"].hide_render = True
for i in range(0, 360, 90):
	bpy.data.objects["Setup"].rotation_euler.z = i / 180 * math.pi
	bpy.context.scene.render.filepath = "render/without_cargo/angle" + str(i) + "/"
	bpy.ops.render.render(animation = True)

bpy.data.objects["Ladung"].hide_render = False
for i in range(0, 360, 90):
	bpy.data.objects["Setup"].rotation_euler.z = i / 180 * math.pi
	bpy.context.scene.render.filepath = "render/with_cargo/angle" + str(i) + "/"
	bpy.ops.render.render(animation = True)