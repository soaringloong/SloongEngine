#include "stdafx.h"
#include "TestPolygonSection6.h"
#include "SloongMath2.h"
using namespace SoaringLoong::Math;
#include "SloongModelLoader.h"
using namespace SoaringLoong::Loader;
#include "Defines.h"
#include "SloongString.h"
using namespace SoaringLoong::Universal;
CTestPolygonSection6::CTestPolygonSection6()
{
}


CTestPolygonSection6::~CTestPolygonSection6()
{
}

void CTestPolygonSection6::Initialize()
{

	vscale = { 1, 1, 1, 1 }, vpos = { 0, 0, 0, 1 }, vrot = { 0, 0, 0, 1 };

	// initialize camera 
	POINT4D  cam_pos = { 0, 40, 0, 1 };
	POINT4D  cam_target = { 0, 0, 0, 1 };
	VECTOR4D cam_dir = { 0, 0, 0, 1 };

	CMath2::Build_Sin_Cos_Tables();



	/*	CPLGLoader::Init_CAM4DV1(&cam,      // the camera object
	CAM_MODEL_EULER, // the euler model
	&cam_pos,  // initial camera position
	&cam_dir,  // initial camera angles
	&cam_target,      // no target
	200.0,      // near and far clipping planes
	12000.0,
	120.0,      // field of view in degrees
	WINDOW_WIDTH,   // size of final screen viewport
	WINDOW_HEIGHT);*/


	CPLGLoader plgLoader(m_pDraw);

	// load the master tank object
	vscale.Initialize(0.75, 0.75, 0.75);
	plgLoader.LoadOBJECT4DV1(&obj_tank, _T("tank2.plg"), &vscale, &vpos, &vrot);

	// load player object for 3rd person view
	vscale.Initialize(0.75, 0.75, 0.75);
	plgLoader.LoadOBJECT4DV1(&obj_player, _T("tank3.plg"), &vscale, &vpos, &vrot);

	// load the master tower object
	vscale.Initialize(1.0, 2.0, 1.0);
	plgLoader.LoadOBJECT4DV1(&obj_tower, _T("tower1.plg"), &vscale, &vpos, &vrot);

	// load the master ground marker
	vscale.Initialize(3.0, 3.0, 3.0);
	plgLoader.LoadOBJECT4DV1(&obj_marker, _T("marker1.plg"), &vscale, &vpos, &vrot);

#define UNIVERSE_RADIUS   4000

	// position the tanks
	for (int index = 0; index < NUM_TANKS; index++)
	{
		// randomly position the tanks
		tanks[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].y = 0; // obj_tank.max_radius;
		tanks[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		tanks[index].w = RAND_RANGE(0, 360);
	} // end for


	// position the towers
	for (int index = 0; index < NUM_TOWERS; index++)
	{
		// randomly position the tower
		towers[index].x = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
		towers[index].y = 0; // obj_tower.max_radius;
		towers[index].z = RAND_RANGE(-UNIVERSE_RADIUS, UNIVERSE_RADIUS);
	} // end for

}

void CTestPolygonSection6::Render()
{

	static MATRIX4X4 mrot;   // general rotation matrix

	// these are used to create a circling camera
	static float view_angle = 0;
	static float camera_distance = 6000;
	static VECTOR4D pos = { 0, 0, 0, 0 };
	static float tank_speed;
	static float turning = 0;

	char work_string[256]; // temp string

	int index; // looping var

	// draw the sky
	m_pDraw->Draw_Rectangle(0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT / 2, RGB(0, 140, 192), m_pDraw->GetBackSurface());

	// draw the ground
	m_pDraw->Draw_Rectangle(0, WINDOW_HEIGHT / 2, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1, RGB(103, 62, 3), m_pDraw->GetBackSurface());

	// read keyboard and other devices here
	//DInput_Read_Keyboard();
	m_pInput->GetInput();
	// game logic here...

	// reset the render list
	CPLGLoader::Reset_RENDERLIST4DV1(&rend_list);

	// allow user to move camera
#define TANK_SPEED        15
	// turbo
	if (m_pInput->IsKeyDown(DIK_SPACE))
		tank_speed = 5 * TANK_SPEED;
	else
		tank_speed = TANK_SPEED;

	// forward/backward
	if (m_pInput->IsKeyDown(DIK_UP))
	{
		// move forward
		cam.pos.x += tank_speed*CMath2::Fast_Sin(cam.dir.y);
		cam.pos.z += tank_speed*CMath2::Fast_Cos(cam.dir.y);
	} // end if

	if (m_pInput->IsKeyDown(DIK_DOWN))
	{
		// move backward
		cam.pos.x -= tank_speed*CMath2::Fast_Sin(cam.dir.y);
		cam.pos.z -= tank_speed*CMath2::Fast_Cos(cam.dir.y);
	} // end if

	// rotate
	if (m_pInput->IsKeyDown(DIK_RIGHT))
	{
		cam.dir.y += 3;

		// add a little turn to object
		if ((turning += 2) > 15)
			turning = 15;

	} // end if

	if (m_pInput->IsKeyDown(DIK_LEFT))
	{
		cam.dir.y -= 3;

		// add a little turn to object
		if ((turning -= 2) < -15)
			turning = -15;

	} // end if
	else // center heading again
	{
		if (turning > 0)
			turning -= 1;
		else
		if (turning < 0)
			turning += 1;

	} // end else

	// generate camera matrix
	CPLGLoader::Build_CAM4DV1_Matrix_Euler(&cam, CAM_ROT_SEQ_ZYX);

	// insert the tanks in the world
	for (index = 0; index < NUM_TANKS; index++)
	{
		// reset the object (this only matters for backface and object removal)
		CPLGLoader::Reset_OBJECT4DV1(&obj_tank);

		// generate rotation matrix around y axis
		CPLGLoader::Build_XYZ_Rotation_MATRIX4X4(0, tanks[index].w, 0, &mrot);

		// rotate the local coords of the object
		CPLGLoader::Transform_OBJECT4DV1(&obj_tank, &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

		// set position of tank
		obj_tank.world_pos.x = tanks[index].x;
		obj_tank.world_pos.y = tanks[index].y;
		obj_tank.world_pos.z = tanks[index].z;

		// attempt to cull object   
		if (!CPLGLoader::Cull_OBJECT4DV1(&obj_tank, &cam, CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			CPLGLoader::Model_To_World_OBJECT4DV1(&obj_tank, TRANSFORM_TRANS_ONLY);

			// insert the object into render list
			CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj_tank, 0);
		} // end if

	} // end for

	// insert the player into the world
	// reset the object (this only matters for backface and object removal)
	CPLGLoader::Reset_OBJECT4DV1(&obj_player);

	// set position of tank
	obj_player.world_pos.x = cam.pos.x + 300 * CMath2::Fast_Sin(cam.dir.y);
	obj_player.world_pos.y = cam.pos.y - 70;
	obj_player.world_pos.z = cam.pos.z + 300 * CMath2::Fast_Cos(cam.dir.y);

	// generate rotation matrix around y axis
	CPLGLoader::Build_XYZ_Rotation_MATRIX4X4(0, cam.dir.y + turning, 0, &mrot);

	// rotate the local coords of the object
	CPLGLoader::Transform_OBJECT4DV1(&obj_player, &mrot, TRANSFORM_LOCAL_TO_TRANS, 1);

	// perform world transform
	CPLGLoader::Model_To_World_OBJECT4DV1(&obj_player, TRANSFORM_TRANS_ONLY);

	// insert the object into render list
	CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj_player, 0);


	// insert the towers in the world
	for (index = 0; index < NUM_TOWERS; index++)
	{
		// reset the object (this only matters for backface and object removal)
		CPLGLoader::Reset_OBJECT4DV1(&obj_tower);

		// set position of tower
		obj_tower.world_pos.x = towers[index].x;
		obj_tower.world_pos.y = towers[index].y;
		obj_tower.world_pos.z = towers[index].z;

		// attempt to cull object   
		if (!CPLGLoader::Cull_OBJECT4DV1(&obj_tower, &cam, CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			CPLGLoader::Model_To_World_OBJECT4DV1(&obj_tower, 0);

			// insert the object into render list
			CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj_tower, 0);
		} // end if

	} // end for

	// seed number generator so that modulation of markers is always the same
	srand(13);

#define POINT_SIZE        200
#define NUM_POINTS_X      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS_Z      (2*UNIVERSE_RADIUS/POINT_SIZE)
#define NUM_POINTS        (NUM_POINTS_X*NUM_POINTS_Z)
	// insert the ground markers into the world
	for (int index_x = 0; index_x < NUM_POINTS_X; index_x++)
	for (int index_z = 0; index_z < NUM_POINTS_Z; index_z++)
	{
		// reset the object (this only matters for backface and object removal)
		CPLGLoader::Reset_OBJECT4DV1(&obj_marker);

		// set position of tower
		obj_marker.world_pos.x = RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_x*POINT_SIZE;
		obj_marker.world_pos.y = obj_marker.max_radius;
		obj_marker.world_pos.z = RAND_RANGE(-100, 100) - UNIVERSE_RADIUS + index_z*POINT_SIZE;

		// attempt to cull object   
		if (!CPLGLoader::Cull_OBJECT4DV1(&obj_marker, &cam, CULL_OBJECT_XYZ_PLANES))
		{
			// if we get here then the object is visible at this world position
			// so we can insert it into the rendering list
			// perform local/model to world transform
			CPLGLoader::Model_To_World_OBJECT4DV1(&obj_marker, 0);

			// insert the object into render list
			CPLGLoader::Insert_OBJECT4DV1_RENDERLIST4DV1(&rend_list, &obj_marker, 0);
		} // end if

	} // end for

	// remove backfaces
	CPLGLoader::Remove_Backfaces_RENDERLIST4DV1(&rend_list, &cam);

	// apply world to camera transform
	CPLGLoader::World_To_Camera_RENDERLIST4DV1(&rend_list, &cam);

	// apply camera to perspective transformation
	CPLGLoader::Camera_To_Perspective_RENDERLIST4DV1(&rend_list, &cam);

	// apply screen transform
	CPLGLoader::Perspective_To_Screen_RENDERLIST4DV1(&rend_list, &cam);

	sprintf_s(work_string, 256, "pos:[%f, %f, %f] heading:[%f] elev:[%f]",
		cam.pos.x, cam.pos.y, cam.pos.z, cam.dir.y, cam.dir.x);

	CString str(work_string);
	m_pDraw->DrawText(str.GetString().c_str(), 0, WINDOW_HEIGHT - 20, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// draw instructions
	m_pDraw->DrawText(_T("Press ESC to exit. Press Arrow Keys to Move. Space for TURBO."), 0, 0, RGB(0, 255, 0), m_pDraw->GetBackSurface());

	// lock the back buffer
	LPBYTE pBackBuffer = m_pDraw->DDraw_Lock_Back_Surface();

	// render the object
	CPLGLoader::Draw_RENDERLIST4DV1_Wire16(&rend_list, pBackBuffer, m_pDraw->GetBackPitch(), m_rcWindow);

	// unlock the back buffer
	m_pDraw->DDraw_Unlock_Back_Surface();

	// flip the surfaces
	//	DDraw_Flip();

	// sync to 30ish fps
	//	Wait_Clock(30);

	// check of user is trying to exit
	// 	if (KEY_DOWN(VK_ESCAPE) || keyboard_state[DIK_ESCAPE])
	// 	{
	// 		PostMessage(main_window_handle, WM_DESTROY, 0, 0);
	// 	} // end if
}
