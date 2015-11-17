import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100

# Münze allein
bpy.data.objects["Coin"].scale = (1.1, 1.1, 1.1)
bpy.data.objects["GreenArrow"].hide_render = True
bpy.data.objects["RedArrow"].hide_render = True
bpy.context.scene.render.filepath = "render/coin.png"
bpy.ops.render.render(write_still = True)

bpy.data.objects["Coin"].scale = (1.0, 1.0, 1.0)

# Münze mit grünem Pfeil
bpy.data.objects["GreenArrow"].hide_render = False
bpy.data.objects["RedArrow"].hide_render = True
bpy.context.scene.render.filepath = "render/coin-green-arrow.png"
bpy.ops.render.render(write_still = True)

# Münze mit rotem Pfeil
bpy.data.objects["GreenArrow"].hide_render = True
bpy.data.objects["RedArrow"].hide_render = False
bpy.context.scene.render.filepath = "render/coin-red-arrow.png"
bpy.ops.render.render(write_still = True)