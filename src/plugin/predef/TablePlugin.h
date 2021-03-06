/*
 * VisionSystem for VisionSorter
 * Copyright (C) 2010, Ben Axnick
 * Ben Axnick <ben@axnick.com.au>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <osg/Vec2>
#include <osg/Vec4>

#include <plugin/Plugin.h>

typedef struct table_settings
{
   void load(const std::string &filename);
   std::string m_TableCfg; //"Data/table.cfg"
   double m_rotateSmoothing; //0.7 // MYSTERY UNITS
   double m_translateSmoothing; //0.7
   double m_tableWidth;// 1200 // MM
   double m_tableHeight;// 1800 // MM
   double m_transmitRate; //0.5 // SECONDS
} TableSettings;



class TrackableSurface
{
public:
   TrackableSurface(CamTracker *tracker, osg::Vec2d dimensions, osg::Vec4d V0, osg::Vec4d n);
   osg::Vec2d Unproject(osg::Vec2d pt);
   bool IsInBounds(osg::Vec2 pt);
   CamTracker* GetTracker();
protected:
   CamTracker *m_tracker;
   osg::Vec4d m_V0;
   osg::Vec4d m_n;
   osg::Vec2d m_dimensions;
};


class TablePlugin : public BasePlugin
{
public:
   TablePlugin(PluginManager *manager);
   bool Init(PluginManager *manager, const std::string &cfgFile);
   void AnnounceClients(std::vector<Plugin*> &clients);

   void IncomingFrame(osgART::GenericVideo* sourceVid, osg::Timer_t now, double elapsed);
   void IncludeInScene(osg::Node* child);

   CamTracker* CanHasTracking();
   TrackableSurface *Surface();

   static TablePlugin* getTableRef(std::vector<Plugin*> &plugins);
private:
   osgART::Marker *m_marker;
   CamTracker *m_camTracker;
   osg::Group *m_sceneGroup;
   TableSettings m_cfg;
   TrackableSurface *m_surface;
};
