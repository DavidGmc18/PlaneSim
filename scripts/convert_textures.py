from PIL import Image
import numpy as np

# Low = shiny; High = rough; default = 1.0
SHININESS_POWER = 0.25
# Low = less metallic; High = more metallic; start = 0.04
DIELECTRIC_SPECULAR = 0.06

base_color = np.array(Image.open("scripts/in/base_color.png").convert("RGB")) / 255.0
roughness = np.array(Image.open("scripts/in/roughness.png").convert("L")) / 255.0
metallic = np.array(Image.open("scripts/in/metallic.png").convert("L")) / 255.0

metallic = np.expand_dims(metallic, axis=-1)
roughness = np.expand_dims(roughness, axis=-1)

def lerp(a, b, t):
    return a * (1.0 - t) + b * t

diffuse = base_color * (1.0 - metallic)
specular = lerp(DIELECTRIC_SPECULAR, base_color, metallic)
shininess = np.pow(1.0 - roughness, SHININESS_POWER).squeeze()

Image.fromarray((diffuse * 255).astype(np.uint8)).save("scripts/out/diffuse.png")
Image.fromarray((specular * 255).astype(np.uint8)).save("scripts/out/specular.png")
Image.fromarray((shininess * 255).astype(np.uint8)).save("scripts/out/shininess.png")