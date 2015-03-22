import bpy
import math

def showTile(objectNameToShow):
    for obj in bpy.data.objects:
        if obj.name.startswith('tile-'):
            obj.hide_render = not(objectNameToShow == obj);

bpy.data.scenes["Scene"].render.resolution_percentage = 100
bpy.data.scenes["Scene"].render.resolution_x = 64
bpy.data.scenes["Scene"].render.resolution_y = 64
bpy.data.cameras["Camera"].ortho_scale = 1.414213562
bpy.data.materials["ShouldNeverBeVisible"].use_transparency = True

for tile_object in bpy.data.objects:
    if not tile_object.name.startswith('tile-'):
        continue

    tile_name = tile_object.name.replace('tile-', '')
    print ('Rendering %s...' % tile_name)
    
    # Kachel ins Kamera-Sichtfeld schieben
    tile_object.location.y = 0
    tile_object.location.x = 0

    showTile(tile_object)

    for angle in range(0, 360, 90):
        bpy.data.objects["Setup"].rotation_euler.z = angle / 180 * math.pi

        bpy.context.scene.render.filepath = "render/%s-angle%u.png" % (tile_name, angle)
        bpy.ops.render.render(write_still = True)
