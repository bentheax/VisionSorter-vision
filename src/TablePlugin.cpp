#include "TablePlugin.h"

#include <osgART/ARTTransform>
#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osg/Geode>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "cam_t.h"

using namespace std;

void TableSettings::load(const std::string &path)
{
    using boost::property_tree::ptree;
    ptree pt;

    read_ini(path, pt);

    m_TableCfg = pt.get("table.cfg_path", string("Data/table.cfg"));
    m_rotateSmoothing = pt.get("table.rotate_smoothing", 0.7);
    m_translateSmoothing = pt.get("table.translate_smoothing", 0.7);
    m_tableWidth = pt.get("table.width", 1200.0);
    m_tableHeight = pt.get("table.height",  1800.0);
    m_transmitRate = pt.get("table.transmit_rate", 0.5);
}

void MarkPoint(osg::Group *scene, const osg::Vec3 &point)
{
    float boxSizeX = 5.0f; float boxSizeY = 5.0f; float boxSizeZ = 30.0f;

    osg::ShapeDrawable *sd = new osg::ShapeDrawable(new osg::Box(
            osg::Vec3(point.x() - boxSizeX / 2, point.y() - boxSizeY / 2, point.z() + boxSizeZ / 2),
            boxSizeX,boxSizeY,boxSizeZ));

    sd->setColor(osg::Vec4(0, 0, 1, 1));
    osg::Geode* basicShapesGeode = new osg::Geode();
    basicShapesGeode->addDrawable(sd);
    scene->addChild(basicShapesGeode);
}

TablePlugin::TablePlugin(PluginManager *manager)
{
    this->BasePlugin::Construct(manager, TABLE_ID, PRIORITY_HIGH);
}


bool TablePlugin::Init(PluginManager *manager, const std::string &cfgFile)
{
    m_cfg.load(cfgFile);

    osg::ref_ptr<osgART::GenericTracker> tracker = m_manager->ConstructTracker(m_cfg.m_TableCfg);
    if (tracker->getMarkerCount() != 1 + 1) // HACK, marker file needs at least two markers defined for some reason
    {
        cout << "Table should track one multi marker. Got: " << tracker->getMarkerCount() << endl;
        return false;
    }

    m_marker = tracker->getMarker(0);
    m_marker->setActive(true);
    m_marker->setRotationalSmoothing(m_cfg.m_rotateSmoothing);
    m_marker->setTranslationalSmoothing(m_cfg.m_translateSmoothing);

    m_camTracker = new CamTracker(m_manager->GetProjection(), m_marker);
    m_surface = new TrackableSurface(m_camTracker, osg::Vec4d(0., 0., 0., 1.), osg::Vec4d(0., 0., 1., 0.));

    osg::ref_ptr<osg::MatrixTransform> transMat = new osgART::ARTTransform(m_marker);

    m_sceneGroup = new osg::Group();

    MarkPoint(m_sceneGroup, osg::Vec3d(m_cfg.m_tableWidth, m_cfg.m_tableHeight, 0));
    MarkPoint(m_sceneGroup, osg::Vec3d(0, m_cfg.m_tableHeight, 0));
    MarkPoint(m_sceneGroup, osg::Vec3d(m_cfg.m_tableWidth, 0, 0));
    MarkPoint(m_sceneGroup, osg::Vec3d(0, 0, 0));

    transMat->addChild(m_sceneGroup);
    m_manager->AddScene(transMat.get());

    return true;
}

TrackableSurface* TablePlugin::Surface()
{
    return m_surface;
}

void TablePlugin::AnnounceClients(std::vector<Plugin*> &clients)
{
    // The table module doesn't care about the others.
}

void TablePlugin::IncomingFrame(osgART::GenericVideo* sourceVid, osg::Timer_t now, double elapsed)
{
    m_elapsedTime += elapsed;

    if (m_elapsedTime < m_cfg.m_transmitRate) return;

    m_elapsedTime = fmod(m_elapsedTime, m_cfg.m_transmitRate);

    cam_t message;
    message.info = m_manager->GetInfo(now);
    message.hasVision = m_marker->isValid();

    lcm_t *comms = m_manager->GetComms();
    cam_t_publish(comms, Id().c_str(), &message); // NOTE: a result of 0 is "success"

}

void TablePlugin::IncludeInScene(osg::Node* child)
{
    m_sceneGroup->addChild(child);
}

CamTracker* TablePlugin::CanHasTracking()
{
    return m_camTracker;
}




TrackableSurface::TrackableSurface(CamTracker *tracker, osg::Vec4d V0, osg::Vec4d n)
    :m_tracker(tracker),
      m_V0(V0),
      m_n(n)
{}

double TrackableSurface::GetHeading()
{
    return m_tracker->FindZRotation();
}

osg::Vec2d TrackableSurface::Unproject(osg::Vec2d pt)
{
    osg::Vec4d result = m_tracker->UnprojectToPlane(pt, m_V0, m_n);
    return osg::Vec2d(result.x(), result.y());
}