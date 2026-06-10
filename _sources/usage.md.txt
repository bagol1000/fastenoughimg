# Usage

## Installation

```bash
git clone https://github.com/bagol1000/fastenoughimg.git
cd fastenoughimg
pip install -e .
```

## Example

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
