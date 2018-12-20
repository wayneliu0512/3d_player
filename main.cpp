#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QFirstPersonCameraController>
#include <QVector2D>

#include <math.h>
#include <vector>

#define PI 3.14159265
#define MODE_CIRCLE 0
#define MODE_LINE 1
#define MODE_INVERSE_LINE 2

Qt3DCore::QEntity *createScene()
{
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    // Sphere
    Qt3DCore::QEntity *center_entity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(1);
    Qt3DExtras::QPhongMaterial *spherematerial = new Qt3DExtras::QPhongMaterial;
    spherematerial->setDiffuse(QColor(Qt::black));
    spherematerial->setAmbient(QColor(Qt::black));
    center_entity->addComponent(spherematerial);
    center_entity->addComponent(sphereMesh);

    // x
    Qt3DCore::QEntity *centerX_entity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QCylinderMesh *cylinderXMesh = new Qt3DExtras::QCylinderMesh;
    cylinderXMesh->setRadius(0.5);
    cylinderXMesh->setLength(10);
    Qt3DCore::QTransform *centerXTransform = new Qt3DCore::QTransform;
    centerXTransform->setRotationZ(90);
    centerXTransform->setTranslation(QVector3D(5,0,0));
    Qt3DExtras::QPhongMaterial *centerXmaterial = new Qt3DExtras::QPhongMaterial;
    centerXmaterial->setDiffuse(QColor(Qt::red));
    centerXmaterial->setAmbient(QColor(Qt::red));

    centerX_entity->addComponent(cylinderXMesh);
    centerX_entity->addComponent(centerXmaterial);
    centerX_entity->addComponent(centerXTransform);

    // y
    Qt3DCore::QEntity *centerY_entity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QCylinderMesh *cylinderYMesh = new Qt3DExtras::QCylinderMesh;
    cylinderYMesh->setRadius(0.5);
    cylinderYMesh->setLength(10);
    Qt3DCore::QTransform *centerYTransform = new Qt3DCore::QTransform;
    centerYTransform->setTranslation(QVector3D(0,5,0));
    Qt3DExtras::QPhongMaterial *centerYmaterial = new Qt3DExtras::QPhongMaterial;
    centerYmaterial->setDiffuse(QColor(Qt::green));
    centerYmaterial->setAmbient(QColor(Qt::green));

    centerY_entity->addComponent(cylinderYMesh);
    centerY_entity->addComponent(centerYmaterial);
    centerY_entity->addComponent(centerYTransform);

    // y
    Qt3DCore::QEntity *centerZ_entity = new Qt3DCore::QEntity(rootEntity);
    Qt3DExtras::QCylinderMesh *cylinderZMesh = new Qt3DExtras::QCylinderMesh;
    cylinderZMesh->setRadius(0.5);
    cylinderZMesh->setLength(10);
    Qt3DCore::QTransform *centerZTransform = new Qt3DCore::QTransform;
    centerZTransform->setRotationX(90);
    centerZTransform->setTranslation(QVector3D(0,0,5));
    Qt3DExtras::QPhongMaterial *centerZmaterial = new Qt3DExtras::QPhongMaterial;
    centerZmaterial->setDiffuse(QColor(Qt::blue));
    centerZmaterial->setAmbient(QColor(Qt::blue));
    centerZ_entity->addComponent(cylinderZMesh);
    centerZ_entity->addComponent(centerZmaterial);
    centerZ_entity->addComponent(centerZTransform);

    return rootEntity;
}

void AddPoint(Qt3DCore::QEntity *root, const QVector3D &vector, const QColor &color) {
    // Sphere
    Qt3DCore::QEntity *center_entity = new Qt3DCore::QEntity(root);
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh;
    sphereMesh->setRadius(0.25f);
    Qt3DExtras::QPhongMaterial *spherematerial = new Qt3DExtras::QPhongMaterial;
    spherematerial->setDiffuse(color);
    spherematerial->setAmbient(color);
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
    transform->setTranslation(vector);

    center_entity->addComponent(spherematerial);
    center_entity->addComponent(sphereMesh);
    center_entity->addComponent(transform);
}

void AddProfile(Qt3DCore::QEntity *root, const std::vector<QVector2D> &data_set,
                const int &mode, const int &total_profiles, const QVector3D &tool_offset) {
    static int profile_count{0};
    static int color_gradiant{0};
    float gap_x{2};
    float gap_y{2};
    float gap_z{2};
    QColor point_color(color_gradiant, color_gradiant, color_gradiant);
    QVector3D point;
    switch (mode) {
    case MODE_LINE: {
        for (auto peak : data_set) {
            point = {profile_count*gap_x, peak.x()*gap_y, peak.y()*gap_z};
            point += tool_offset;
            AddPoint(root, point, point_color);
        }
    }
        break;
    case MODE_INVERSE_LINE: {
        for (auto peak : data_set) {
            point = {-profile_count*gap_x, peak.x()*gap_y, peak.y()*gap_z};
            point += tool_offset;
            AddPoint(root, point, point_color);
        }
    }
        break;
    case MODE_CIRCLE: {
        auto start_angle {std::atan(static_cast<double>(tool_offset.y()/tool_offset.x())) * 180.0 / PI};
        if (tool_offset.x() <= 0) start_angle += 180;
        auto angle = start_angle + ((360/static_cast<double>(total_profiles))
                     *static_cast<double>(profile_count));
        double cos_angle{std::cos(angle*PI/180.0)};
        double sin_angle{std::sin(angle*PI/180.0)};

        QVector3D tool_offset_new {tool_offset.length()*static_cast<float>(cos_angle),
                                   tool_offset.length()*static_cast<float>(sin_angle),
                                   tool_offset.z()};
        for(auto peak : data_set) {
            point = {peak.x()*gap_x*static_cast<float>(cos_angle),
                    peak.x()*gap_y*static_cast<float>(sin_angle),
                    peak.y()*gap_z};
            point += tool_offset_new;
            AddPoint(root, point, point_color);
        }
    }
        break;
    }

    color_gradiant <= 255 ? color_gradiant+=14 : color_gradiant = 0 ;
    ++profile_count;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;
    view.defaultFrameGraph()->setClearColor(QColor(QRgb(0x4d4d4f)));
    Qt3DCore::QEntity *scene = createScene();

    // Camera
    Qt3DRender::QCamera *camera = view.camera();
//    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 150.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QFirstPersonCameraController *camController
            = new Qt3DExtras::QFirstPersonCameraController(scene);
    camController->setCamera(camera);
    camController->setLinearSpeed(100);

    view.setRootEntity(scene);

    //----------------------------------------------------------------------------------------
    // create fake points
    std::vector<std::vector<QVector2D>> data_set;
    int profile_size = 19;
    int peak_size = 19;

    int start_peak = -(peak_size/2);

    for (int i = 0; i < profile_size; ++i) {
        std::vector<QVector2D> profile;
        int peak_offset = start_peak;
        for (int j = 0; j < peak_size; ++j) {
                profile.push_back(QVector2D(peak_offset++, i));
        }
        data_set.push_back(profile);
    }

    QVector3D tool_offset{8,8,0};

    for(auto profile : data_set) {
        AddProfile(scene, profile, MODE_CIRCLE, profile_size, tool_offset);
    }
    //----------------------------------------------------------------------------------------

    view.show();
    return app.exec();
}
