#include "ogre_application.h"
#include "bin/path_config.h"

namespace ogre_application {

/* Some configuration constants */
/* They are written here as global variables, but ideally they should be loaded from a configuration file */

/* Initialization */
const Ogre::String config_filename_g = "";
const Ogre::String plugins_filename_g = "";
const Ogre::String log_filename_g = "Ogre.log";

/* Main window settings */
const Ogre::String window_title_g = "Demo";
const Ogre::String custom_window_capacities_g = "";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

/* Viewport and camera settings */
float viewport_width_g = 0.95f;
float viewport_height_g = 0.95f;
float viewport_left_g = (1.0f - viewport_width_g) * 0.5f;
float viewport_top_g = (1.0f - viewport_height_g) * 0.5f;
unsigned short viewport_z_order_g = 100;
const Ogre::ColourValue viewport_background_color_g(0.0, 0.0, 0.0);
float camera_near_clip_distance_g = 0.1;
float camera_far_clip_distance_g = 5000.0;
Ogre::Vector3 camera_position_g(0.0, -10.0, 800.0);
Ogre::Vector3 camera_look_at_g(0.0, 0.0, 0.0);
Ogre::Vector3 camera_up_g(0.0, 1.0, 0.0);
 
/* Materials */
const Ogre::String material_directory_g = MATERIAL_DIRECTORY;


OgreApplication::OgreApplication(void){

    /* Don't do work in the constructor, leave it for the Init() function */
}


void OgreApplication::Init(void){

	/* Set default values for the variables */
	animating_ = true;
	space_down_ = false;

	input_manager_ = NULL;
	keyboard_ = NULL;
	mouse_ = NULL;

	/* Camera demo */
	last_dir_ = Direction::Forward;
	num_asteroids_ = 0;
	counter = 0;
	dirction = Ogre::Vector3(0,0,0);
	/* Run all initialization steps */
    InitRootNode();
    InitPlugins();
    InitRenderSystem();
    InitWindow();
    InitViewport();
	InitEvents();
	InitOIS();
	LoadMaterials();
}


void OgreApplication::InitRootNode(void){

    try {
		
		/* We need to have an Ogre root to be able to access all Ogre functions */
        ogre_root_ = std::auto_ptr<Ogre::Root>(new Ogre::Root(config_filename_g, plugins_filename_g, log_filename_g));
		//ogre_root_->showConfigDialog();

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
		throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitPlugins(void){

    try {

		/* Load plugin responsible for OpenGL render system */
        Strings plugin_names;
        plugin_names.push_back("RenderSystem_GL");
		
        Strings::iterator iter = plugin_names.begin();
        Strings::iterator iter_end = plugin_names.end();
        for (; iter != iter_end; iter++){
            Ogre::String& plugin_name = (*iter);
            if (OGRE_DEBUG_MODE){
                plugin_name.append("_d");
            }
            ogre_root_->loadPlugin(plugin_name);
        }

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitRenderSystem(void){

    try {

        const Ogre::RenderSystemList& render_system_list = ogre_root_->getAvailableRenderers();
        if (render_system_list.size() == 0)
        {
			throw(OgreAppException(std::string("OgreApp::Exception: Sorry, no rendersystem was found.")));
        }

        Ogre::RenderSystem *render_system = render_system_list.at(0);
        ogre_root_->setRenderSystem(render_system);

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}

        
void OgreApplication::InitWindow(void){

    try {

        /* Create main window for the application */
		bool create_window_automatically = false;
        ogre_root_->initialise(create_window_automatically, window_title_g, custom_window_capacities_g);

        Ogre::NameValuePairList params;
        params["FSAA"] = "0";
        params["vsync"] = "true";
        ogre_window_ = ogre_root_->createRenderWindow(window_title_g, window_width_g, window_height_g, window_full_screen_g, &params);

        ogre_window_->setActive(true);
        ogre_window_->setAutoUpdated(false);
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitViewport(void){

    try {

        /* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->createSceneManager(Ogre::ST_GENERIC, "MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

        /* Create camera object */
        Ogre::Camera* camera = scene_manager->createCamera("MyCamera");
        Ogre::SceneNode* camera_scene_node = root_scene_node->createChildSceneNode("MyCameraNode");
        camera_scene_node->attachObject(camera);

		camera->setNearClipDistance(camera_near_clip_distance_g);
        camera->setFarClipDistance(camera_far_clip_distance_g); 

		camera->setPosition(camera_position_g);
		camera->lookAt(camera_look_at_g);
		camera->setFixedYawAxis(true, camera_up_g);

        /* Create viewport */
        Ogre::Viewport *viewport = ogre_window_->addViewport(camera, viewport_z_order_g, viewport_left_g, viewport_top_g, viewport_width_g, viewport_height_g);

        viewport->setAutoUpdated(true);
        viewport->setBackgroundColour(viewport_background_color_g);

		/* Set aspect ratio */
        float ratio = float(viewport->getActualWidth()) / float(viewport->getActualHeight());
        camera->setAspectRatio(ratio);
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitEvents(void){

	try {

		/* Add this object as a FrameListener for render events (see frameRenderingQueued event) */
		ogre_root_->addFrameListener(this);

		/* Add this object as a WindowEventListener to handle the window resize event */
		Ogre::WindowEventUtilities::addWindowEventListener(ogre_window_, this);

	}
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
		throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitOIS(void){

	/* Initialize the Object Oriented Input System (OIS) */
	try {

		/* Initialize input manager */
		OIS::ParamList pl; // Parameter list passed to the input manager initialization
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		ogre_window_->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		pl.insert(std::make_pair(std::string("w32_mouse"), 
		std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), 
		std::string("DISCL_NONEXCLUSIVE")));
		input_manager_ = OIS::InputManager::createInputSystem(pl);

		/*size_t hWnd = 0;
		ogre_window_->getCustomAttribute("WINDOW", &hWnd);
		input_manager_ = OIS::InputManager::createInputSystem(hWnd);*/

		/* Initialize keyboard and mouse */
		keyboard_ = static_cast<OIS::Keyboard*>(input_manager_->createInputObject(OIS::OISKeyboard, false));

		mouse_ = static_cast<OIS::Mouse*>(input_manager_->createInputObject(OIS::OISMouse, false));
		unsigned int width, height, depth;
		int top, left;
		ogre_window_->getMetrics(width, height, depth, left, top);
		const OIS::MouseState &ms = mouse_->getMouseState();
		ms.width = width;
		ms.height = height;

	}
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::LoadMaterials(void){

    try {
		
		/* Load materials that can then be assigned to objects in the scene */
		Ogre::String resource_group_name = "MyGame";
		Ogre::ResourceGroupManager& resource_group_manager = Ogre::ResourceGroupManager::getSingleton();
		resource_group_manager.createResourceGroup(resource_group_name);
		bool is_recursive = false;
		resource_group_manager.addResourceLocation(material_directory_g, "FileSystem", resource_group_name, is_recursive);
		resource_group_manager.initialiseResourceGroup(resource_group_name);
		resource_group_manager.loadResourceGroup(resource_group_name);

	}
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::CreateCube(void){

	try {
		/* Create a cube */

		/* Retrieve scene manager and root scene node */
		Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
		Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

		/* Create the 3D object */
        Ogre::ManualObject* object = NULL;
        Ogre::String object_name = "Cube";
        object = scene_manager->createManualObject(object_name);
        object->setDynamic(false);

        /* Create triangle list for the object */
		Ogre::String material_name = "ObjectMaterial";
        object->begin(material_name, Ogre::RenderOperation::OT_TRIANGLE_LIST);

		/* Vertices of a cube */
		Ogre::Vector3 v0(-0.5, -0.5,  0.5);
		Ogre::Vector3 v1( 0.5, -0.5,  0.5);
		Ogre::Vector3 v2( 0.5,  0.5,  0.5);
		Ogre::Vector3 v3(-0.5,  0.5,  0.5);
		Ogre::Vector3 v4(-0.5, -0.5, -0.5);
		Ogre::Vector3 v5( 0.5, -0.5, -0.5);
		Ogre::Vector3 v6( 0.5,  0.5, -0.5);
		Ogre::Vector3 v7(-0.5,  0.5, -0.5);

		/* Normal of each face of the cube */
		Ogre::Vector3 n0( 0.0,  0.0,  1.0);
		Ogre::Vector3 n1( 1.0,  0.0,  0.0);
		Ogre::Vector3 n2( 0.0,  0.0, -1.0);
		Ogre::Vector3 n3(-1.0,  0.0,  0.0);
		Ogre::Vector3 n4( 0.0,  1.0,  0.0);
		Ogre::Vector3 n5( 0.0, -1.0,  0.0);

		/* Cube's vertex colors */
		Ogre::ColourValue clr0(0.0, 0.0, 1.0);
		Ogre::ColourValue clr1(1.0, 0.0, 1.0);
		Ogre::ColourValue clr2(1.0, 1.0, 1.0);
		Ogre::ColourValue clr3(0.0, 1.0, 0.0);
		Ogre::ColourValue clr4(0.0, 0.0, 1.0);
		Ogre::ColourValue clr5(1.0, 0.0, 0.0);
		Ogre::ColourValue clr6(1.0, 1.0, 0.0);
		Ogre::ColourValue clr7(0.0, 1.0, 0.0);
		
		/* This construction only partially uses shared vertices, so that we can assign appropriate vertex normals
		   to each face */
		/* Each face of the cube is defined by four vertices (with the same normal) and two triangles */
		object->position(v0);
		object->normal(n0);
		object->textureCoord(0, 0);
		object->colour(clr0);

		object->position(v1);
		object->normal(n0);
		object->textureCoord(1, 1);
		object->colour(clr1);

		object->position(v2);
		object->normal(n0);
		object->textureCoord(1, 1);
		object->colour(clr2);

		object->position(v3);
		object->normal(n0);
		object->textureCoord(0, 1);
		object->colour(clr3);
		
		object->position(v1);
		object->normal(n1);
		object->textureCoord(0, 0);
		object->colour(clr1);

		object->position(v5);
		object->normal(n1);
		object->textureCoord(1, 0);
		object->colour(clr5);

		object->position(v6);
		object->normal(n1);
		object->textureCoord(1, 1);
		object->colour(clr6);

		object->position(v2);
		object->normal(n1);
		object->textureCoord(0, 1);
		object->colour(clr2);

		object->position(v5);
		object->normal(n2);
		object->textureCoord(0, 0);
		object->colour(clr5);

		object->position(v4);
		object->normal(n2);
		object->textureCoord(1, 0);
		object->colour(clr4);
		
		object->position(v7);
		object->normal(n2);
		object->textureCoord(1, 1);
		object->colour(clr7);

		object->position(v6);
		object->normal(n2);
		object->textureCoord(0, 1);
		object->colour(clr6);

		object->position(v4);
		object->normal(n3);
		object->textureCoord(0, 0);
		object->colour(clr4);

		object->position(v0);
		object->normal(n3);
		object->textureCoord(1, 0);
		object->colour(clr0);

		object->position(v3);
		object->normal(n3);
		object->textureCoord(1, 1);
		object->colour(clr3);

		object->position(v7);
		object->normal(n3);
		object->textureCoord(0, 1);
		object->colour(clr7);

		object->position(v3);
		object->normal(n4);
		object->textureCoord(0, 0);
		object->colour(clr3);

		object->position(v2);
		object->normal(n4);
		object->textureCoord(1, 0);
		object->colour(clr2);

		object->position(v6);
		object->normal(n4);
		object->textureCoord(1, 1);
		object->colour(clr6);

		object->position(v7);
		object->normal(n4);
		object->textureCoord(0, 1);
		object->colour(clr7);

		object->position(v1);
		object->normal(n5);
		object->textureCoord(0, 0);
		object->colour(clr1);

		object->position(v0);
		object->normal(n5);
		object->textureCoord(1, 0);
		object->colour(clr0);

		object->position(v4);
		object->normal(n5);
		object->textureCoord(1, 1);
		object->colour(clr4);

		object->position(v5);
		object->normal(n5);
		object->textureCoord(0, 1);
		object->colour(clr5);

		for (int i = 0; i < 6; i++){
			object->triangle(i*4 + 0, i*4 + 1, i*4 + 3);
			object->triangle(i*4 + 1, i*4 + 2, i*4 + 3);
		}
   
		/* We finished the object */
        object->end();
		
        /* Convert triangle list to a mesh */
        Ogre::String mesh_name = "Cube";
        object->convertToMesh(mesh_name);

	}
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::CreateIcosahedron(void){

	try {
		/* Retrieve scene manager and root scene node */
		Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
		Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

		/* Create the 3D object */
        Ogre::ManualObject* object = NULL;
        Ogre::String object_name = "Icosahedron";
        object = scene_manager->createManualObject(object_name);
        object->setDynamic(false);

        /* Create triangle list for the object */
		Ogre::String material_name = "ObjectMaterial";
        object->begin(material_name, Ogre::RenderOperation::OT_TRIANGLE_LIST);

		/* Vertices of an icosahedron */
		#define X 0.525731112119133606
		#define Z 0.850650808352039932
        static float vdata[12][3] = {
	        {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
	        {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
	        {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}};
 
		/* Vertex colors */
		static float clr[12][3] = {
			{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}, {1.0, 1.0, 0.0},
			{1.0, 0.0, 1.0}, {0.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, {0.6, 0.4, 0.2},
			{1.0, 0.2, 0.8}, {1.0, 0.4, 0.0}, {0.0, 0.6, 0.0}, {0.6, 0.6, 0.6}};

        /* Faces */
        static int tindices [20][3] = {
	        {1, 4, 0}, {4, 9, 0}, {4, 5, 9}, {8, 5, 4},	{1, 8, 4}, 
	        {1, 10, 8}, {10, 3, 8}, {8, 3, 5}, {3, 2, 5}, {3, 7, 2}, 
	        {3, 10, 7}, {10, 6, 7}, {6, 11, 7}, {6, 0, 11},	{6, 1, 0}, 
	        {10, 1, 6}, {11, 0, 9}, {2, 11, 9}, {5, 2, 9}, {11, 2, 7}, 
        };
		
		/* Add vertices and faces */
		for (int i = 0; i < 12; i++){
			object->position(Ogre::Vector3(vdata[i][0], vdata[i][1], vdata[i][2]));
			object->normal(Ogre::Vector3(vdata[i][0], vdata[i][1], vdata[i][2]));
			object->colour(Ogre::ColourValue(clr[i][0], clr[i][1], clr[i][2]));
		}

		for (int i = 0; i < 20; i++){
			object->triangle(tindices[i][0], tindices[i][1], tindices[i][2]);
		}
   
		/* We finished the object */
        object->end();
		
        /* Convert triangle list to a mesh */
        Ogre::String mesh_name = "Icosahedron";
        object->convertToMesh(mesh_name);

	}
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::MainLoop(void){

    try {

        /* Main loop to keep the application going */

        ogre_root_->clearEventTimes();

        while(!ogre_window_->isClosed()){
            ogre_window_->update(false);

            ogre_window_->swapBuffers();

            ogre_root_->renderOneFrame();

            Ogre::WindowEventUtilities::messagePump();
        }
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}

void OgreApplication::windowResized(Ogre::RenderWindow* rw){

	/* Update the window and aspect ratio when the window is resized */
	int width = rw->getWidth(); 
    int height = rw->getHeight();
      
    Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
    Ogre::Camera* camera = scene_manager->getCamera("MyCamera");

	if (camera != NULL){
		camera->setAspectRatio((double)width/height);
    }

	const OIS::MouseState &ms = mouse_->getMouseState();
    ms.width = width;
    ms.height = height;

	ogre_window_->resize(width, height);
	ogre_window_->windowMovedOrResized();
	ogre_window_->update();
}


void OgreApplication::CreateAsteroidField(int num_asteroids){

	try {
		/* Check number of asteroids requested */
		if (num_asteroids > MAX_NUM_ASTEROIDS){
			num_asteroids_ = MAX_NUM_ASTEROIDS;
		} else {
			num_asteroids_ = num_asteroids;
		}

		/* Create asteroid field */
		for (int i = 0; i < num_asteroids_; i++){
            asteroid_[i].pos = Ogre::Vector3(-300 + 600 * (rand() % 1000) / 1000.0f, -300 + 600 * (rand() % 1000) / 1000.0f, 600 * (rand() % 1000) / 1000.0f);
			asteroid_[i].ori = Ogre::Quaternion(1.0f, 3.14*(rand() % 1000) / 1000.0f, 3.14*(rand() % 1000) / 1000.0f, 3.14*(rand() % 1000) / 1000.0f);
			asteroid_[i].lm = Ogre::Quaternion(1.0f, 0.005*3.14*(rand() % 1000) / 1000.0f, 0.005*3.14*(rand() % 1000) / 1000.0f, 0.005*3.14*(rand() % 1000) / 1000.0f);
			asteroid_[i].drift = Ogre::Vector3(((double) rand() / RAND_MAX)*0.2, ((double) rand() / RAND_MAX)*0.2, ((double) rand() / RAND_MAX)*0.2);
        }

		/* Create multiple entities for the asteroids */

        /* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();
		Ogre::Camera* camera = scene_manager->getCamera("MyCamera");

        /* Create multiple entities of a mesh */
		Ogre::String entity_name, prefix("Asteroid");
		for (int i = 0; i < num_asteroids_; i++){
			/* Create entity */
			entity_name = prefix + Ogre::StringConverter::toString(i);
			//Ogre::Entity *entity = scene_manager->createEntity(entity_name, "Cube");
			Ogre::Entity *entity = scene_manager->createEntity(entity_name, "Icosahedron");

			/* Create a scene node for the entity */
			/* The scene node keeps track of the entity's position */
			cube_[i] = root_scene_node->createChildSceneNode(entity_name);
			cube_[i]->attachObject(entity);
		}
		Ogre::Entity *entity = scene_manager->createEntity("MoveCube","Cube");
		cube_laser_ = root_scene_node->createChildSceneNode("CubeNode");
		cube_laser_->attachObject(entity);
		cube_laser_->scale(0.2,0.2,200);
		

		Ogre::Entity *entity1 = scene_manager->createEntity("TargetCube","Cube");
		cube_target_ = root_scene_node->createChildSceneNode("CubeNode1");
		cube_target_->attachObject(entity1);
		cube_target_->scale(0.2,0.2,0.2);

		laserFire(camera->getOrientation(), camera->getPosition());

    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}

bool OgreApplication::frameRenderingQueued(const Ogre::FrameEvent& fe){
  
	/* This event is called after a frame is queued for rendering */
	/* Do stuff in this event since the GPU is rendering and the CPU is idle */

	/* Keep animating if flag is on */
	if (animating_){
		/* Animate transformation */
		TransformAsteroidField();
	}

	/* Capture input */
	keyboard_->capture();
	mouse_->capture();

	/* Handle specific key events */
	if (keyboard_->isKeyDown(OIS::KC_SPACE)){
		space_down_ = true;
	}
	if ((!keyboard_->isKeyDown(OIS::KC_SPACE)) && space_down_){
		animating_ = !animating_;
		space_down_ = false;
	}
	if (keyboard_->isKeyDown(OIS::KC_ESCAPE)){
        ogre_root_->shutdown();
        ogre_window_->destroy();
        return false;
    }

	/* Camera demo */
	if (!animating_){
		return true;
	}
	/* Get camera object */
	Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
	Ogre::Camera* camera = scene_manager->getCamera("MyCamera");
	if (!camera){
		return false;
	}
	
	/* Move ship according to keyboard input and last move */
	/* Movement factors to apply to the ship */
	double trans_factor = 5.0; // Small continuous translation
	double small_trans_factor = 1.0; // Translation applied with thrusters
	Ogre::Radian rot_factor(Ogre::Math::PI / 180); // Camera rotation with directional thrusters
	/*make camera */
	camera->setPosition(camera->getPosition() + dirction);

	/* Apply user commands */
	/* Camera rotation (thruster) */
	if (keyboard_->isKeyDown(OIS::KC_UP)){
		camera->pitch(rot_factor);
	}
	
	if (keyboard_->isKeyDown(OIS::KC_DOWN)){
		camera->pitch(-rot_factor);
	}

	if (keyboard_->isKeyDown(OIS::KC_LEFT)){
		camera->yaw(rot_factor);
	}

	if (keyboard_->isKeyDown(OIS::KC_RIGHT)){
		camera->yaw(-rot_factor);
	}

	if (keyboard_->isKeyDown(OIS::KC_S)){
		camera->roll(-rot_factor);
	}

	if (keyboard_->isKeyDown(OIS::KC_X)){
		camera->roll(rot_factor);
	}

	/* Camera translation */
	if (keyboard_->isKeyDown(OIS::KC_A)){
		dirction += camera->getDirection() * 0.1;
	}

	if (keyboard_->isKeyDown(OIS::KC_Z)){
		dirction -= camera->getDirection() * 0.1;
	}

	if (keyboard_->isKeyDown(OIS::KC_PGUP)){
        dirction += camera->getUp() * 0.1;
	}
	
	if (keyboard_->isKeyDown(OIS::KC_PGDOWN)){
        dirction -= camera->getUp() * 0.1;
	}
 
	if (keyboard_->isKeyDown(OIS::KC_COMMA)){
        dirction += camera->getRight() * 0.1;
	}
	
	if (keyboard_->isKeyDown(OIS::KC_PERIOD)){
		dirction -= camera->getRight() * 0.1;
	}

	laserFire(camera->getOrientation(), camera->getPosition());
	
	//laser fire button
	if (keyboard_->isKeyDown(OIS::KC_V)){
		collision();
		cube_laser_->setVisible(true);
		cube_target_->setVisible(false);
	}else{
		cube_laser_->setVisible(false);
		cube_target_->setVisible(true);
	}
	//move cube (targeting cube )
	//if (keyboard_->isKeyDown(OIS::KC_I)){
	//	dirction += camera->getOrientation() * 0.1;
	//}
	//if (keyboard_->isKeyDown(OIS::KC_K)){
	//	moveCube(0.0, -0.05, 0.0);//down
	//}
	//if (keyboard_->isKeyDown(OIS::KC_J)){
	//	moveCube(-0.05, 0.0, 0.0);//left
	//}
	//if (keyboard_->isKeyDown(OIS::KC_L)){
	//	dirction -= camera->getRight() * 0.1;
	//}

	/* Reset spaceship position */
	if (keyboard_->isKeyDown(OIS::KC_R)){
		camera->setPosition(0.0, 0.0, 800.0);
		camera->setOrientation(Ogre::Quaternion::IDENTITY);
		dirction = Ogre::Vector3(0,0,0);
	}
 
    return true;
}

void OgreApplication::TransformAsteroidField(void){
	//create move cube
	Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
	
	// Rotate asteroids
    for (int i = 0; i < num_asteroids_; i++){
		// Set orientation
		asteroid_[i].ori = asteroid_[i].lm * asteroid_[i].ori;
		cube_[i]->setOrientation(asteroid_[i].ori);
		
		// Could add some drift as well
		//asteroid_[i].pos += asteroid_[i].drift;

		// Set the position every time
		cube_[i]->setPosition(asteroid_[i].pos);
    }
}

void OgreApplication::laserFire(Ogre::Quaternion value, Ogre::Vector3 pos )
{
		Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();
		Ogre::Camera* camera = scene_manager->getCamera("MyCamera");

	    cube_laser_->setPosition(pos - camera->getUp());
		cube_laser_->setOrientation(value);

		cube_target_->setPosition(pos - camera->getUp() + camera->getDirection()*15);
		cube_target_->setOrientation(value);
}



void OgreApplication::CreateTargetingCube (void){

	try {
		/* Create multiple entities of a Cylinder */

        /* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

		//create first cylinder which is called A as center
		Ogre::Entity *entity0 = scene_manager->createEntity("MoveCube", "Cube");
		cube_[0] = root_scene_node->createChildSceneNode("MoveCube");
		cube_[0]->attachObject(entity0);
		cube_[0]->scale(1.0,1.0,1.0);	
		
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}



void OgreApplication::moveCube(float x, float y , float z)
{
	 //Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
  //   Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();
	 //Ogre::Camera* camera = scene_manager->getCamera("MyCamera");
	 //Ogre::Vector3 f0(0,0,0);
	 //Ogre::Radian u0(0);
	 //
	 //x += 0.01;
	 //y += 0.01;
	 //z += 0.01;
	 //q = cube_target_->getOrientation();
	 //cube_target_->rotate(f0, u0);
	 //Ogre:: Quaternion p(0, f0);
	 //p (0, f0);
	 
}


void OgreApplication::collision()
{
	Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
	Ogre::Camera* camera = scene_manager->getCamera("MyCamera");
	Ogre::Vector3 l = camera->getDirection();
	Ogre::Vector3 o = camera->getPosition();

	int r = 1;
	for(int i=0; i< MAX_NUM_ASTEROIDS; i++)
	{
		Ogre::Vector3 c = asteroid_[i].pos;
		Ogre::Vector3 dir = c - o;
		float length = dir.length();
		float value = l.dotProduct(dir)*l.dotProduct(dir)- length*length + r*r;
		if(value > 0){
			Ogre::String entity_name, prefix("Asteroid");
			entity_name = prefix + Ogre::StringConverter::toString(i);
			Ogre::SceneNode* root_scene_node = scene_manager->getSceneNode(entity_name);			
			root_scene_node->detachAllObjects();
		}
	}

}


} // namespace ogre_application;