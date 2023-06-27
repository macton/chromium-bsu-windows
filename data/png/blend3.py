# blend3 a = (r+g+b)/3
from PIL import Image as image
in_image       = image.open("electric.png")
in_image_size  = in_image.size
out_image      = image.new(mode="RGBA",size=in_image_size)
in_image_rgb   = in_image.convert("RGB")
in_image_data  = in_image_rgb.getdata()
out_image_data = []
width          = in_image_size[0]
height         = in_image_size[1]
for y in range(height):
    for x in range(width):
        index   = (y*width)+x
        in_rgb  = in_image_data[index]
        r       = int(in_rgb[0])
        g       = int(in_rgb[1])
        b       = int(in_rgb[2])
        a       = int((r+g+b)/3)
        out_image_data.append(tuple([r,g,b,a]))
out_image.putdata(out_image_data)
out_image.save("electric_a.png")
