# blend1 a = max((r+g+b),0xff)
from PIL import Image as image
in_name        = "stat-top.png"
out_name       = "stat-top_a.png"

in_image       = image.open(in_name)
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
        a       = max(int((r+g+b)),0xff)
        out_image_data.append(tuple([r,g,b,a]))
out_image.putdata(out_image_data)
out_image.save(out_name)
