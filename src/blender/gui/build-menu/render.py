import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100

# ohne Outline
bpy.context.scene.node_tree.nodes["Outline-Switch"].check = False
bpy.context.scene.render.filepath = \
    "render/" + bpy.path.display_name_from_filepath(bpy.context.blend_data.filepath) + "-normal.png"
bpy.ops.render.render(write_still = True)

# mit Outline
bpy.context.scene.node_tree.nodes["Outline-Switch"].check = True
bpy.context.scene.render.filepath = \
    "render/" + bpy.path.display_name_from_filepath(bpy.context.blend_data.filepath) + "-pressed.png"
bpy.ops.render.render(write_still = True)
