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

for tile_object in bpy.data.objects:
    if not tile_object.name.startswith('tile-'):
        continue

    tile_name = tile_object.name.replace('tile-', '')
    print ('Rendering %s...' % tile_name)
    
    bpy.data.objects["Kamera"].location.x = 10 + tile_object.location.y
    bpy.data.objects["Kamera"].location.y = 10 - tile_object.location.x

    showTile(tile_object)

    # Kachel leicht vergrößern, damit es die Ränder nicht halbtransparent sind 
    # und die Kacheln somit nicht zusammenpassen
    tile_object.scale.x = 1.1;
    tile_object.scale.y = 1.1;

    bpy.context.scene.render.filepath = "render/%s.png" % tile_name
    bpy.ops.render.render(write_still = True)