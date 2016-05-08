import bpy

# Rauch backen
bpy.ops.ptcache.bake_all(bake = True)

# Jetzt erst rendern
exec(compile(open("render.py").read(), "render.py", 'exec'))