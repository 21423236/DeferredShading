![Header Image](https://github.com/davidlwesten/DeferredShading/tree/master/images/title.png "Title")

# ***Deferred Shading***



## Overview

When beginning this project I decided to start writing a 3D RenderingSystem completely from scratch as I figured that on top of learningthe Deferred Rendering Algorithm, it would be a good exercise. Indesigning my system, I wanted to abstract the notion of a Rendererand a Render Pass in order to accommodate for future extensions tothe project. The application and the scene it contains are completelyagnostic to how the final image is produced. Thus making it extremelysimple to exchange Renderers (Forward Lighting, Visibility Buffer,etc.) on the fly.

Asit currently stands, each frame is rendered by the Deferred Rendererin four subsequent passes, each contributing its portion to the imagedisplayed on the screen. The first pass, which I called GeometryPass,  traverses the Scene Graph and fills the G-Buffer with allinformation necessary to perform the BRDF lighting calculation whilealso creating a collection of global and local lights used insubsequent passes. The second pass, which I termed Shadow Pass, takesthe collection of global lights collected during the Geometry Passand produces a shadow map for each. The third pass, which I calledAmbient Pass, uses the G-Buffer’s Kd Render Target and a constantenvironment value to produce the base to which other lighting passesaccumulate. The fourth pass, called the Lighting Pass, is actuallytwo distinct passes that process global lights and local lightsrespectively. The last and final pass, the Tone Mapping Pass,performs the HDR to LDR conversion as well as the Gamma correction.

## Specifications

### Geometry Pass (Pass1):

Aspreviously mentioned, the Geometry Pass’ responsibility is  collectall necessary surface information into the G-Buffer whilesimultaneously creating collections of Global Lights and Local Lightsfrom the Scene. The Deferred Renderer maintains the G-Buffer which iscomprised of four color render targets plus one depth. 

1.   The first color buffer holds the world position’s x, y, z, and w	components in their respective r, g, b, and a channels:

     ![color1](https://github.com/davidlwesten/DeferredShading/tree/master/images/color1.png)

2.   The second color buffer of the G-Buffer contains the transformednormals’ x, y, and z components in the buffers r, g, and b channelsleaving the alpha channel permanently set to 1:

       ![color2](https://github.com/davidlwesten/DeferredShading/tree/master/images/color2.png)

(Although the plane on which the dragons stand appears to be a complexgeometry, it has a normal map applied creating the illusion ofcontour)

3.  The third color buffer contains the surface material’s Kd r, g,and b values in the buffer’s r, g, and b channels also leaving thealpha channel permanently set to 1:

    ![color3](https://github.com/davidlwesten/DeferredShading/tree/master/images/color3.png)

4.  The fourth and final color buffer contains the surface material’sKs r, g, and b values in the buffer’s r, g, and b channels whilestoring the material’s alpha value in the buffer’s alpha channel:

     ![color4](https://github.com/davidlwesten/DeferredShading/tree/master/images/color4.png)


### Shadow Pass (Pass 2):

The second pass generates a shadow map for each Global Light contained inthe scene by rendering the Scene from the perspective of the lightsources and storing the depths of the all visible objects in theShadow Buffer’s render target. The scene that is being used toillustrate the various stages of the Deferred Renderer’s pipelinecontains two global lights.

|         Global Light 1         |         Global Light 2         |
| :----------------------------: | :----------------------------: |
| ![shadow2](https://github.com/davidlwesten/DeferredShading/tree/master/images/shadow2.png) | ![shadow1](https://github.com/davidlwesten/DeferredShading/tree/master/images/shadow1.png) |

### Ambient Pass (Pass 3):

Theambient pass lays the groundwork for all subsequent lighting passes.It is the first  of three passes to additively blend their resultsinto the Light Accumulation Buffer. There are two reasons I separatedthe ambient term in the lighting equation from the Global and Locallight passes. Since at this stage we consider the ambient light to bea constant within the scene, it would not make sense to have it bedependent on the number of lights contained in the scene and so itmust be separate. Second, I figured with Ambient Occlusion on thehorizon it would make everything easier to implement this newfeature. Although at this point it is anticlimactic, below is thestate of the Light Accumulation Buffer after the Ambient Pass.   

![ambient](https://github.com/davidlwesten/DeferredShading/tree/master/images/ambient.png)

### Lighting Pass (Pass 3):

As the name would indicate, this is where most of the magic happens. Asindicated before, this pass is split into two consecutive passes; onefor global lights and one for local.

#### Global Lighting Pass (Pass 3a):

Forevery Global Light in the scene, this pass renders a Full ScreenQuadrilateral and takes the world position, world normal, kd, ks, andalpha values from the G-Buffer as input and uses them to perform theBRDF lighting calculation. Additionally, the shader program takes theShadow Map, generated by the Shadow Pass, as input to determinewhether a given pixel is illuminated by its global light source.Below are the states of the Light Accumulation Buffer after the twoglobal lights in the example scene:

|         Global Light 1         |         Global Light 2         |
| :----------------------------: | :----------------------------: |
| ![global1](https://github.com/davidlwesten/DeferredShading/tree/master/images/global1.png) | ![global2](https://github.com/davidlwesten/DeferredShading/tree/master/images/global2.png) |



#### Local Lighting Pass (Pass 3b):

Thispass is the final of the lighting passes and must be capable ofrendering thousands and up to tens-of-thousands of lights. In orderto achieve this, this Deferred Renderer employs OpenGL’s built-incapability of instanced rendering. The idea behind it is that ifthere is a need to render a certain mesh many times during a frame,having to call a glDraw* function for each instance is wasteful dueto the repeated overhead. Instead, it is encouraged to store allnecessary data associated with each instance in the GPU’s memory toallow it to optimize rendering however it sees fit. I created anobject that maintains a buffer of all local lighting information (thelight’s position, intensity, and influence radius) that is uploadedonto the GPU which is then used to render all local lights in asingle API draw call. The geometric object used to render the lightvolumes is an icosahedron. I decided that it would be computationallycheaper to render a rougher approximation of a sphere rather than atraditional UV-sphere, especially when considering that 40,000 mightbe have to transformed per frame. Additionally, the shader programthat processes the pixels generated by the icosahedrons uses anattenuation factor to weaken the influence of the light as itapproaches the limit of its influence.

![local](https://github.com/davidlwesten/DeferredShading/tree/master/images/local.png) 

### Tone Mapping Pass(Pass 4):

Asis evident from the results of the last pass, the frame produced upto this point appears heavily over exposed and must go through afinal processing step that accomplishes two things: converting a highdynamic range image to a low dynamic range, and calculate the gammacorrection expected by the hardware’s pipeline. This pass producesthe final output:

![tone](https://github.com/davidlwesten/DeferredShading/tree/master/images/tone.png)



## Capabilities

Thescene which I’m using the demonstrate the rendering system iscontained within a cube that is 4 x 4 x 4. The system can easilyrender up to 5,000 local lights with medium to large radii ofinfluence and maintain real-time standards. Beyond this points, itcan handle 10,000 to 40,000 local lights with small to medium radiiof influence.

10,000 Local Lights:

![cap1](https://github.com/davidlwesten/DeferredShading/tree/master/images/cap1.png)



40,000 Local Lights:

![cap2](https://github.com/davidlwesten/DeferredShading/tree/master/images/cap2.png)



## Optimizations

Aswas briefly mentioned in my report, I employed a couple ofoptimizations available through the OpenGL API. After having writtena couple of shader programs for the different passes, I grew annoyedwith having to resubmit the same Uniform data to different programs.After a little research online, I starting using OpenGL’s UniformBuffer Objects that conveniently allow the sharing of uniform databetween various programs while only having to send it to the GPUonce. I also made use of Shader Storage Buffers in order to enablethe use of instancing. I created a buffer object in the GPU’smemory that holds all local light information (world position,intensity, and radius of influence) which greatly reduces the copyoperations overhead by a factor of the number of lights. In thefuture I plan to optimize this system further by tracking whichlights have changed since the last frame in order to only have toupdate small chunks in the buffer object.



## Other Features

Asidefrom the Deferred Rendering aspect, the application offers varioustypes of editors. A Scene Graph Editor, Material Editor, and TextureManager (picture below) to name a few.

|          Scene Editor          |         Material Editor          |        Texture Manager         |
| :----------------------------: | :------------------------------: | :----------------------------: |
| ![matedit](https://github.com/davidlwesten/DeferredShading/tree/master/images/matedit.png) | ![scenedit](https://github.com/davidlwesten/DeferredShading/tree/master/images/scenedit.png) | ![texedit](https://github.com/davidlwesten/DeferredShading/tree/master/images/texedit.png) |





