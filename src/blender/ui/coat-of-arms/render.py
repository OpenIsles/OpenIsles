import bpy
import math

def show(blenderObjectToShow):
    for obj in bpy.data.objects:
        if obj.name.startswith('coat-of-arms-'):
            obj.hide_render = not(blenderObjectToShow == obj);

bpy.data.scenes["Scene"].render.resolution_percentage = 100

for blender_object in bpy.data.objects:
    if not blender_object.name.startswith('coat-of-arms-'):
        continue

    object_name = blender_object.name.replace('coat-of-arms-', '')
    print ('Rendering %s...' % object_name)
    
    show(blender_object)

    bpy.data.scenes["Scene"].render.resolution_x = 32
    bpy.data.scenes["Scene"].render.resolution_y = 40
    bpy.context.scene.render.filepath = "render/large/%s.png" % object_name
    bpy.ops.render.render(write_still = True)

    bpy.data.scenes["Scene"].render.resolution_x = 22
    bpy.data.scenes["Scene"].render.resolution_y = 28
    bpy.context.scene.render.filepath = "render/small/%s.png" % object_name
    bpy.ops.render.render(write_still = True)