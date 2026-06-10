# Usage

## Installation

```bash
git clone https://github.com/bagol1000/fastenoughimg.git
cd fastenoughimg
pip install -e .
```

## Examples of use

```python
import numpy as np
import fastenoughimg as fimg

img = np.array(...)

inverted = fimg.pyinvert(img)
gray = fimg.pyrgb_to_gray(img)
blurred = fimg.pyblur_gauss(gray, sigma=2.0)
black_white, threshold = fimg.pythreshold_otsu(gray)
with_detected_edges = fimg.pyedge_detect(gray)
```
## Examples on images

**Original**

![original](_static/jezioro.jpg) 

**Invert**

![inverted](_static/jezioro_negatyw.png)

**Gaussian blur** `(sigma=20)`

![blur](_static/jezioro_blur.png)

**Edge detection**

![edges](_static/jezioro_edges.png)
