import bpy
import math

def showTile(objectNameToShow):
    for obj in ["Gerade", "Kurve", "T", "Kreuzung"]:
        bpy.data.objects[obj].hide_render = not(objectNameToShow == obj);
    

bpy.data.scenes["Scene"].render.resolution_percentage = 100

bpy.data.objects["Setup"].rotation_euler.z = 0 / 180 * math.pi

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera oben"]
showTile("Kurve")
bpy.context.scene.render.filepath = "render/curve0.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera rechts"]
showTile("Kreuzung")
bpy.context.scene.render.filepath = "render/cross.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera unten"]
showTile("T")
bpy.context.scene.render.filepath = "render/tee0.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera links"]
showTile("Gerade")
bpy.context.scene.render.filepath = "render/straight0.png"
bpy.ops.render.render(write_still = True)

bpy.data.objects["Setup"].rotation_euler.z = 90 / 180 * math.pi

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera oben"]
showTile("Gerade")
bpy.context.scene.render.filepath = "render/straight90.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera rechts"]
showTile("Kurve")
bpy.context.scene.render.filepath = "render/curve90.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera links"]
showTile("T")
bpy.context.scene.render.filepath = "render/tee90.png"
bpy.ops.render.render(write_still = True)

bpy.data.objects["Setup"].rotation_euler.z = 180 / 180 * math.pi

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera oben"]
showTile("T")
bpy.context.scene.render.filepath = "render/tee180.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera unten"]
showTile("Kurve")
bpy.context.scene.render.filepath = "render/curve180.png"
bpy.ops.render.render(write_still = True)

bpy.data.objects["Setup"].rotation_euler.z = 270 / 180 * math.pi

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera rechts"]
showTile("T")
bpy.context.scene.render.filepath = "render/tee270.png"
bpy.ops.render.render(write_still = True)

bpy.data.scenes["Scene"].camera = bpy.data.objects["Kamera links"]
showTile("Kurve")
bpy.context.scene.render.filepath = "render/curve270.png"
bpy.ops.render.render(write_still = True)
