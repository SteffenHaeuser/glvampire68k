Documentation to gl4Vampire
---------------------------

0. History

V0.01 First version which MIGHT be able to do something.

1. Abstract

This software is a partial OpenGL Implementation for the Maggie-Chip as used in the Apollo Vampire.

a) Why "Partial" ?

The Maggie-Chip currently lacks features for a full OpenGL Implementation. Once more features will be available this Software will be expanded to be closer to a full OpenGL Implementation.

b) Why by Steffen Häuser? Isn't he doing PowerPC Amiga Software ?

Yes, I like the PowerPC Amigas best. But I also like the Apollo Vampire. It is an absolute cool Amiga! I wanted to do this to support this cool system. 
Currently the plan is to get it far enough to run the Quake 2 engine (not yet there, but hopefully soon).

2. How to use it, what special things are needed ?

Due to the way maggie.library and the Maggie Chip work some minor changes to standard OpenGL were needed. But adapting OpenGL Software for gl4vampire should be easy.

a) Startup/Shutdown

You need to call gluOpenDisplayTags.

	struct TagItem maggieTags[6];
	
	maggieTags[0].ti_Tag = VAMPOD_BPP;
	maggieTags[0].ti_Data = 2;
	maggieTags[1].ti_Tag = VAMPOD_MODE;
	maggieTags[1].ti_Data = MAGGIE_MODE;
	maggieTags[2].ti_Tag = VAMPOD_WIDTH;
	maggieTags[2].ti_Data = 640;
	maggieTags[3].ti_Tag = VAMPOD_HEIGHT;
	maggieTags[3].ti_Data = 360;
	maggieTags[4].ti_Tag = VAMPOD_USEINT;
	maggieTags[4].ti_Data = 1;
	maggieTags[5].ti_Tag = TAG_DONE;
	maggieTags[5].ti_Data = 0;
	
	int res = gluOpenDisplayTags(maggieTags);
	if (!res)
	{
		printf("gluOpenDisplay() failed\n");
		return 0;
	}
	
	VAMPOD_BPP: Should be 2 (16 Bit) or 4 (32 Bit)
	VAMPOD_MODE: Should be a valid Maggie-Mode (if you use GL through intuition Windows this will be ignored, but still needs to be set)
	VAMPOD_WIDTH/HEIGHT: Need to be set to a width/height compatible to the Maggie_Mode you used. So if it was 0x0b02 it should be 640 and 360 for example.
	VAMPOD_USEINT: Set to 1 if you want to use a VBL to synchronize (all done internally), else set to 0 or leave out. USEINT does not support running several GL Apps at the same time,
	               if you set it to 0 several GL Apps at the same time should work fine.
	VAMPOD_WINDOW: GL should open an intuition window instead of banging the hardware
	VAMPOD_WINHANDLE: If you already had a Window opened yourselves, just provide the struct Window * to this. Do not specify VAMPOD_WINDOW in this case.
	VAMPOD_WINTITLE: The title for the window, if you use VAMPOD_WINDOW.
	
	You close the display again with gluCloseDisplay().

Note you need to call glViewport right after this, and you need to use the same width/height for it as you used with gluOpenDisplay. The x/y values will be ignored.

b) Frame Handling

Different to standard OpenGL you need to call gluBeginFrame() and gluEndFrame() every frame. Else it won't work.

c) What is missing ?

- glDepthFunc, glAlphaFunc and glBlendFunc are not yet supported and will probably even later only support some modes
- glDepthMask currently disables the Z Buffer completely
- TexEnv, texParameter 	texSubImage2D, Fogging, glGetString and glShadeModel are missing
- No VertexBuffer, IndexBuffer and Multitexturing support yet
- I added a texture format GL_DXT1 for the Maggie native format which is NOT part of OpenGL normally. You can use it with glTexImage2D.
- Currently it is the only format supported, OpenGL compatible formats will come later.
- glCullFace only supports GL_FRONT
- glPolygonMode only supprorts the default stuff
- glDrawBuffer does nothing, but triplebuffering is handled automatically with gl4vampire. You do not need glDrawBuffer for it.
- glEnable/glDisable currently does nothing.
- glScissors is still unimplemented
- The Matrix handling currently (especially per frame overhead) is currently probably slow
- glDeleteTextures currently can only delete one texture in one call
- glGetFloatv currently only supports getting the world matrix
- Drawing functions might need optimizations
- parameter types of GL Calls need to be made more compatible to OpenGL Standard (GLenum instead of int etc.).
- Possiblly I forgot some other current limitations

2. How to link

You need to link with libgl4vampire.a AND libGL.a. The libGL.a will eventually be moved into a Shared Library, the other one will stay a static link library (it contains the GLU functions).
