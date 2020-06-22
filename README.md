# CUDA swap yolo master 420 #

Tontada para cambiar la versiond de CUDA on the fly  
Se acabo tener el \.bashrc de turno lleno de PATHS y basukito

## Set up ##
Lo de siempre
<pre><code>git clone https://github.com/alfosucan/cuda-version.git
cd cuda-version
cmake -S . -B build
cd build
sudo make install
</code></pre>

El ejecutable se instala en /usr/local/bin/  
Para que tire hay que darle `sudo` por lo de los permisos y eso. Por ejemplo,  
    `sudo cuda-version 10.1`
