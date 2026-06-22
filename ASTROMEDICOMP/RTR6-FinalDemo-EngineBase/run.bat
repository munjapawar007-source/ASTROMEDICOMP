@echo off

rd /s /q intermediate
rd /s /q bin

if not exist intermediate mkdir intermediate
if not exist bin mkdir bin

cl.exe /c /EHsc ^
    /I C:\glew-2.1.0\include ^
    /I src ^
    /I external\include ^
    /I external\ffmpeg\include ^
    /I DEMO\public ^
    Main.cpp ^
    src\E_shader.cpp ^
    src\E_camera.cpp ^
    src\E_mesh.cpp ^
    src\E_meshCache.cpp ^
    src\E_material.cpp ^
    src\E_sceneObject.cpp ^
    src\E_utils.cpp ^
    src\E_modelLoader.cpp ^
    src\E_anim.cpp ^
    src\E_inputs.cpp ^
    src\E_scene.cpp ^
    src\E_fog.cpp ^
    src\E_postProcess.cpp ^
    src\E_directionalLight.cpp ^
    src\E_ambientLight.cpp ^
    src\E_spotLight.cpp ^
    src\E_pointLight.cpp ^
    src\E_emitter.cpp ^
    src\E_spline.cpp ^
    src\E_particles.cpp ^
    src\E_fbo.cpp ^
    src\E_water.cpp ^
    src\E_uniformCache.cpp ^
    src\E_video.cpp ^
    DEMO\private\DEMO.cpp ^
    "DEMO\private\00-AMCIntro.cpp" ^
    "DEMO\private\00-IntroScene.cpp" ^
    "DEMO\private\01-MumbaiDarshan.cpp" ^
    "DEMO\private\02-CSTInterior.cpp" ^
    "DEMO\private\03-TajDinningToTVBlast.cpp" ^
    "DEMO\private\04-BedroomScene.cpp" ^
    "DEMO\private\05-CamaHospital.cpp" ^
    "DEMO\private\06-EndMessageAndCredits.cpp" ^
    /Fo:intermediate\

rc.exe /Fo intermediate\Window.res engineResources\base\Window.rc

link.exe ^
    intermediate\Main.obj ^
    intermediate\E_shader.obj ^
    intermediate\E_camera.obj ^
    intermediate\E_mesh.obj ^
    intermediate\E_meshCache.obj ^
    intermediate\E_material.obj ^
    intermediate\E_sceneObject.obj ^
    intermediate\E_utils.obj ^
    intermediate\E_modelLoader.obj ^
    intermediate\E_anim.obj ^
    intermediate\E_inputs.obj ^
    intermediate\E_scene.obj ^
    intermediate\E_fog.obj ^
    intermediate\E_postProcess.obj ^
    intermediate\E_directionalLight.obj ^
    intermediate\E_ambientLight.obj ^
    intermediate\E_spotLight.obj ^
    intermediate\E_pointLight.obj ^
    intermediate\E_emitter.obj ^
    intermediate\E_spline.obj ^
    intermediate\E_particles.obj ^
    intermediate\E_fbo.obj ^
    intermediate\E_water.obj ^
    intermediate\E_uniformCache.obj ^
    intermediate\E_video.obj ^
    intermediate\DEMO.obj ^
    "intermediate\00-AMCIntro.obj" ^
    "intermediate\00-IntroScene.obj" ^
    "intermediate\01-MumbaiDarshan.obj" ^
    "intermediate\02-CSTInterior.obj" ^
    "intermediate\03-TajDinningToTVBlast.obj" ^
    "intermediate\04-BedroomScene.obj" ^
    "intermediate\05-CamaHospital.obj" ^
    "intermediate\06-EndMessageAndCredits.obj" ^
    intermediate\Window.res ^
    /LIBPATH:C:\glew-2.1.0\lib\Release\x64 ^
    /LIBPATH:external\ffmpeg\lib ^
    avformat.lib ^
    avcodec.lib ^
    avutil.lib ^
    swscale.lib ^
    User32.lib ^
    GDI32.lib ^
    /SUBSYSTEM:WINDOWS ^
    /OUT:bin\SWEET.exe

copy /Y external\ffmpeg\bin\avcodec-62.dll   bin\ >nul
copy /Y external\ffmpeg\bin\avdevice-62.dll  bin\ >nul
copy /Y external\ffmpeg\bin\avfilter-11.dll  bin\ >nul
copy /Y external\ffmpeg\bin\avformat-62.dll  bin\ >nul
copy /Y external\ffmpeg\bin\avutil-60.dll    bin\ >nul
copy /Y external\ffmpeg\bin\swresample-6.dll bin\ >nul
copy /Y external\ffmpeg\bin\swscale-9.dll    bin\ >nul

bin\SWEET.exe
