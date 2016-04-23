import bpy

def show_object(object_name_to_show):
    for obj in bpy.data.objects:
        obj_for_name = obj
        if not(obj_for_name.name.startswith('icon-')):
            obj_for_name = obj_for_name.parent

        if obj_for_name is None:
            return

        obj.hide_render = (object_name_to_show != obj_for_name.name)


bpy.data.scenes["Scene"].render.resolution_percentage = 100

for object in bpy.data.objects:
    if not object.name.startswith('icon-'):
        continue

    object_name = object.name.replace('icon-', '')
    print ('Rendering icon for %s...' % object_name)

    show_object(object.name)

    bpy.context.scene.render.filepath = "render/%s.png" % object_name
    bpy.ops.render.render(write_still = True)
