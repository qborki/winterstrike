import sys
import bpy
import math
import tempfile
import subprocess

with tempfile.TemporaryDirectory() as tempdir:
    argv  = sys.argv
    argv  = argv[argv.index("--") + 1:]
    sides = int(argv[1])

    for side in range(0, sides):
        bpy.context.scene.objects['Origin'].rotation_euler[2] = -side * (2 * math.pi / sides)

        bpy.context.scene.render.filepath = tempdir + '/' + str(side) + '-'
        bpy.ops.render.render( animation=True )

    subprocess.call(['montage', '-geometry', '+0+0', '-background', 'none', '-tile', 'x' + str(sides), tempdir + '/*.png', argv[0]])
