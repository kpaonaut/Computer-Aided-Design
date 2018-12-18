/**
 * @file main.cpp
 * @brief Simple Physics Simulation main
 *
 * @author Eric Butler (edbutler)
 * @author Derek Basehore (dbasehor)
 */


#include "application/application.hpp"
#include "application/camera_roam.hpp"
#include "application/imageio.hpp"
#include "application/scene_loader.hpp"
#include "application/opengl.hpp"
#include "scene/scene.hpp"

#include <iostream>
#include <cstring>

namespace _462 {

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

#define BUFFER_SIZE(w,h) ( (size_t) ( 4 * (w) * (h) ) )

#define KEY_SCREENSHOT SDLK_f

// pretty sure these are sequential, but use an array just in case
static const GLenum LightConstants[] = {
    GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3,
    GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7
};
static const size_t NUM_GL_LIGHTS = 8;

// renders a scene using opengl
static void render_scene( const Scene& scene );

/**
 * Struct of the program options.
 */
struct Options
{
    // whether to open a window or just render without one
    bool open_window;
    // not allocated, pointed it to something static
    const char* input_filename;
    // not allocated, pointed it to something static
    const char* output_filename;
    // window dimensions
    int width, height;
};

class PhysicsApplication : public Application
{
public:

    PhysicsApplication( const Options& opt )
        : options( opt ), buffer( 0 ), buf_width( 0 ), buf_height( 0 ) { }
    virtual ~PhysicsApplication() { free( buffer ); }

    virtual bool initialize();
    virtual void destroy();
    virtual void update( real_t );
    virtual void render();
    virtual void handle_event( const SDL_Event& event );

    // the scene to render
    Scene scene;

    // options
    Options options;

    // the camera
    CameraRoamControl camera_control;

    bool pause;
    real_t speed;

    // the image buffer for raytracing
    unsigned char* buffer;
    // width and height of the buffer
    int buf_width, buf_height;
};

bool PhysicsApplication::initialize()
{
    // copy camera into camera control so it can be moved via mouse
    camera_control.camera = scene.camera;
    bool load_gl = options.open_window;
    pause = false;
    speed = 1.0;

    try {

        Material* const* materials = scene.get_materials();
        Mesh* const* meshes = scene.get_meshes();

        // load all textures
        for ( size_t i = 0; i < scene.num_materials(); ++i ) {
            if ( !materials[i]->load() || ( load_gl && !materials[i]->create_gl_data() ) ) {
                std::cout << "Error loading texture, aborting.\n";
                return false;
            }
        }

        // load all meshes
        for ( size_t i = 0; i < scene.num_meshes(); ++i ) {
            if ( !meshes[i]->load() || ( load_gl && !meshes[i]->create_gl_data() ) ) {
                std::cout << "Error loading mesh, aborting.\n";
                return false;
            }
        }

    } catch ( std::bad_alloc const& ) {
        std::cout << "Out of memory error while initializing scene\n.";
        return false;
    }

    // set the gl state
    if ( load_gl ) {
        float arr[4];
        arr[3] = 1.0; // alpha is always 1

        glClearColor(
            scene.background_color.r,
            scene.background_color.g,
            scene.background_color.b,
            1.0f );

        scene.ambient_light.to_array( arr );
        glLightModelfv( GL_LIGHT_MODEL_AMBIENT, arr );

        const PointLight* lights = scene.get_lights();

        for ( size_t i = 0; i < NUM_GL_LIGHTS && i < scene.num_lights(); i++ ) {
            const PointLight& light = lights[i];
            glEnable( LightConstants[i] );
            light.color.to_array( arr );
            glLightfv( LightConstants[i], GL_DIFFUSE, arr );
            glLightfv( LightConstants[i], GL_SPECULAR, arr );
            glLightf( LightConstants[i], GL_CONSTANT_ATTENUATION, light.attenuation.constant );
            glLightf( LightConstants[i], GL_LINEAR_ATTENUATION, light.attenuation.linear );
            glLightf( LightConstants[i], GL_QUADRATIC_ATTENUATION, light.attenuation.quadratic );
        }

        glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );
    }

    return true;
}

void PhysicsApplication::destroy()
{

}

void PhysicsApplication::update( real_t delta_time )
{
    // copy camera over from camera control (if not raytracing)
    camera_control.update( delta_time );
    scene.camera = camera_control.camera;

    static const size_t NUM_ITER = 20;

    // step the simulation
    if ( !pause ) {
        real_t step_size = delta_time / NUM_ITER;
        for ( size_t i = 0; i < NUM_ITER; i++ ) {
            scene.update( step_size * speed );
        }
    }
}

void PhysicsApplication::render()
{
    int width, height;

    // query current window size, resize viewport
    get_dimension( &width, &height );
    glViewport( 0, 0, width, height );

    // fix camera aspect
    Camera& camera = scene.camera;
    camera.aspect = real_t( width ) / real_t( height );

    // clear buffer
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // reset matrices
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    render_scene( scene );
    glPopAttrib();
}

void PhysicsApplication::handle_event( const SDL_Event& event )
{
    camera_control.handle_event( this, event );

    switch ( event.type )
    {
    case SDL_KEYDOWN:
        switch ( event.key.keysym.sym )
        {
        case KEY_SCREENSHOT:
            take_screenshot();
            break;
        case SDLK_UP:
            speed *= 2;
            break;
        case SDLK_DOWN:
            speed /= 2;
            break;
        case SDLK_SPACE:
            pause = !pause;
            break;
        default:
            break;
        }
    default:
        break;
    }
}

static void render_scene( const Scene& scene )
{
    // backup state so it doesn't mess up raytrace image rendering
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );

    glClearColor(
        scene.background_color.r,
        scene.background_color.g,
        scene.background_color.b,
        1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glEnable( GL_NORMALIZE );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHTING );
    glEnable( GL_TEXTURE_2D );

    // set camera transform

    const Camera& camera = scene.camera;

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( camera.get_fov_degrees(),
                    camera.get_aspect_ratio(),
                    camera.get_near_clip(),
                    camera.get_far_clip() );

    const Vector3& campos = camera.get_position();
    const Vector3 camref = camera.get_direction() + campos;
    const Vector3& camup = camera.get_up();

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt( campos.x, campos.y, campos.z,
               camref.x, camref.y, camref.z,
               camup.x,  camup.y,  camup.z );
    // set light data
    float arr[4];
    arr[3] = 1.0; // w is always 1

    scene.ambient_light.to_array( arr );
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT, arr );

    glLightModeli( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

    const PointLight* lights = scene.get_lights();

    for ( size_t i = 0; i < NUM_GL_LIGHTS && i < scene.num_lights(); i++ ) {
        const PointLight& light = lights[i];
        glEnable( LightConstants[i] );
        light.color.to_array( arr );
        glLightfv( LightConstants[i], GL_DIFFUSE, arr );
        glLightfv( LightConstants[i], GL_SPECULAR, arr );
        glLightf( LightConstants[i], GL_CONSTANT_ATTENUATION, light.attenuation.constant );
        glLightf( LightConstants[i], GL_LINEAR_ATTENUATION, light.attenuation.linear );
        glLightf( LightConstants[i], GL_QUADRATIC_ATTENUATION, light.attenuation.quadratic );
        light.position.to_array( arr );
        glLightfv( LightConstants[i], GL_POSITION, arr );
    }
    // render each object

    Geometry* const* geometries = scene.get_geometries();

    for ( size_t i = 0; i < scene.num_geometries(); ++i ) {
        const Geometry& geom = *geometries[i];
        Vector3 axis;
        real_t angle;

        glPushMatrix();

        glTranslated( geom.position.x, geom.position.y, geom.position.z );
        geom.orientation.to_axis_angle( &axis, &angle );
        glRotated( angle * ( 180.0 / PI ), axis.x, axis.y, axis.z );
        glScaled( geom.scale.x, geom.scale.y, geom.scale.z );

        geom.render();

        glPopMatrix();
    }

    glPopClientAttrib();
    glPopAttrib();
}

} /* _462 */

using namespace _462;

/**
 * Prints program usage. XXX change
 */
static void print_usage( const char* progname )
{
    std::cout << "Usage: " << progname << " [-r] [-d width height] input_scene [output_file]\n"
        "\n" \
        "Options:\n" \
        "\n" \
        "\t-r:\n" \
        "\t\tRaytraces the scene and saves to the output file without\n" \
        "\t\tloading a window or creating an opengl context.\n" \
        "\t-d width height\n" \
        "\t\tThe dimensions of image to raytrace (and window if using\n" \
        "\t\tand opengl context. Defaults to width=800, height=600.\n" \
        "\tinput_scene:\n" \
        "\t\tThe scene file to load and raytrace.\n" \
        "\toutput_file:\n" \
        "\t\tThe output file in which to write the rendered images.\n" \
        "\t\tIf not specified, default timestamped filenames are used.\n" \
        "\n" \
        "Instructions:\n" \
        "\n" \
        "\tPress 'r' to raytrace the scene. Press 'r' again to go back to\n" \
        "\tgo back to OpenGL rendering. Press 'f' to dump the most recently\n" \
        "\traytraced image to the output file.\n" \
        "\n" \
        "\tUse the mouse and 'w', 'a', 's', 'd', 'q', and 'e' to move the\n" \
        "\tcamera around. The keys translate the camera, and left and right\n" \
        "\tmouse buttons rotate the camera.\n" \
        "\n" \
        "\tIf not using windowed mode (i.e., -r was specified), then output\n" \
        "\timage will be automatically generated and the program will exit.\n" \
        "\n";
}


/**
 * Parses args into an Options struct. Returns true on success, false on failure.
 */
static bool parse_args( Options* opt, int argc, char* argv[] )
{
    int input_index = 1;

    if ( argc < 2 ) {
        print_usage( argv[0] );
        return false;
    }

    if ( strcmp( argv[1], "-r" ) == 0 ) {
        opt->open_window = false;
        ++input_index;
    } else {
        opt->open_window = true;
    }

    if ( argc <= input_index ) {
        print_usage( argv[0] );
        return false;
    }

    // check if it's a -d, if so then get window dimensions
    if ( strcmp( argv[input_index], "-d" ) == 0 ) {
        if ( argc <= input_index + 3 ) {
            print_usage( argv[0] );
            return false;
        }

        // parse window dimensions
        opt->width = -1;
        opt->height = -1;
        sscanf( argv[input_index + 1], "%d", &opt->width );
        sscanf( argv[input_index + 2], "%d", &opt->height );
        // check for valid width/height
        if ( opt->width < 1 || opt->height < 1 ) {
            std::cout << "Invalid window dimensions\n";
            return false;
        }

        input_index += 3;
    } else {
        opt->width = DEFAULT_WIDTH;
        opt->height = DEFAULT_HEIGHT;
    }

    opt->input_filename = argv[input_index];

    if ( argc > input_index + 1 ) {
        opt->output_filename = argv[input_index + 1];
    } else {
        opt->output_filename = 0;
    }

    if ( argc > input_index + 2 ) {
        std::cout << "Too many arguments.\n";
        return false;
    }

    return true;
}

int main( int argc, char* argv[] )
{
    Options opt;

    Matrix3 mat;
    Matrix4 trn;
    make_transformation_matrix( &trn, Vector3::Zero, Quaternion::Identity, Vector3( 2, 2, 2 ) );

    make_normal_matrix( &mat, trn );

    if ( !parse_args( &opt, argc, argv ) ) {
        return 1;
    }

    PhysicsApplication app( opt );

    // load the given scene
    if ( !load_scene( &app.scene, opt.input_filename ) ) {
        std::cout << "Error loading scene " << opt.input_filename << ". Aborting.\n";
        return 1;
    }

    // setup running the physics simulation
    real_t fps = 30.0;
    const char* title = "15462 Project 5 - Physics";
    // start a new application
    return Application::start_application( &app, opt.width, opt.height, fps, title );
}

