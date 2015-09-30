import bpy
import math

bpy.data.scenes["Scene"].render.resolution_percentage = 100

def render(index):
    for i in range(0, 360, 45):
        bpy.data.objects["Setup"].rotation_euler.z = i / 180 * math.pi
        bpy.context.scene.render.filepath = "render/angle" + str(i) + "/sheep" + str(index) + "-"
        bpy.ops.render.render(animation = True)

def select_material_slot_by_name(material_name):
    for material_slot_index in range(len(bpy.data.objects["Sheep"].material_slots)):
        if bpy.data.objects["Sheep"].material_slots[material_slot_index].name == material_name:
            bpy.data.objects["Sheep"].active_material_index = material_slot_index


# sheep1: weißes Fell

render(1)


# sheep2: alle Fell-Faces durch volles Fell ersetzen

bpy.context.scene.objects.active = bpy.data.objects["Sheep"]
bpy.ops.object.mode_set(mode='EDIT')
bpy.ops.mesh.select_all(action='DESELECT')

select_material_slot_by_name("Fell (weiß)")
bpy.ops.object.material_slot_select()

select_material_slot_by_name("Fell (voll)")
bpy.ops.object.material_slot_assign()

# vor dem Rendern Edit-Mode verlassen, sonst werden die Änderungen nicht gerendert!
bpy.ops.object.mode_set(mode='OBJECT')

render(2)


# sheep0: alle Fell-Faces durch "Fell (geschoren)" ersetzen

bpy.ops.object.mode_set(mode='EDIT')

select_material_slot_by_name("Fell (geschoren)")
bpy.ops.object.material_slot_assign()

# vor dem Rendern Edit-Mode verlassen, sonst werden die Änderungen nicht gerendert!
bpy.ops.object.mode_set(mode='OBJECT')

render(0)