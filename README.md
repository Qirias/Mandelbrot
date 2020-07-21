# Calculating the Mandelbrot set using a compute shader.
This project is my introduction to compute shaders with OpenGL.
# Prerequisites
You'll need the g++ compiler and the glfw libary.<br><br>
For **DEBIAN**
```
sudo apt-get install g++
sudo apt-get install libglfw
sudo apt-get install libglfw-dev
```

For **MANJARO**
```
sudo pacman -S g++
sudo pacman -S glfw-x11
```

**Clone the project** <br>
```git clone https://github.com/Qirias/Mandelbrot.git```

**And finally to compile and run the program**<br>
```g++ main.cpp -lGL -lglfw && ./a.out```
# Controls
* **UP/DOWN/RIGHT/LEFT** arrows are for the movement.
* **I/U** increase and decrease the number of iterations by 10, respectively.
* **Z/CTRL+Z** zooming and unzooming.
* **L** shows the two triangles the texture is applied on. <br><br>
**If you wanna know how this works check Iñigo's Quilez paper** [Rendering Worlds with Two Triangles](https://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf)

# Showcase
![Screenshot_2020-07-21_17-17-05](https://user-images.githubusercontent.com/49881189/88053554-a72af580-cb64-11ea-9e18-f7d7ce5c6277.png)
![Screenshot_2020-07-21_17-18-02](https://user-images.githubusercontent.com/49881189/88053598-b742d500-cb64-11ea-8e69-376e46ab5aee.png)
![Peek 2020-07-21 17-14](https://user-images.githubusercontent.com/49881189/88053623-c3c72d80-cb64-11ea-8b20-c7eb58fbd7bf.gif)
