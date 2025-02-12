from setuptools import setup, Extension
import os

HERE = "."

@(lambda f: f())
def audio_ext():
    path = os.path.join(HERE, "audio")
    src = []

    with os.scandir(path) as scandir:
        for item in scandir:
            if (item.name.endswith((".c", ".cpp"))):
                src.append(os.path.join(path, item.name))

    return Extension("_audio", src, include_dirs=[os.path.join(HERE, "include")])

setup(
    ext_modules=[audio_ext]
)