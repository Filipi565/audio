from setuptools import setup, Extension
import os

HERE = os.path.abspath(os.path.join(__file__, ".."))

@(lambda f: f())
def audio_ext():
    path = os.path.join(HERE, "audio")
    src = []

    with os.scandir(path) as scandir:
        for item in scandir:
            if (item.name.endswith((".c", ".cpp"))):
                src.append(item.path)

    return Extension("_audio", src, include_dirs=[os.path.join(HERE, "include")])

setup(
    ext_modules=[audio_ext]
)