#pragma once

#include <geometry_msgs/PointStamped.h>
#include <interactive_markers/interactive_marker_server.h>
#include <interactive_markers/menu_handler.h>
#include <ros/ros.h>
#include <rviz/display_group.h>
#include <rviz/properties/bool_property.h>
#include <rviz/properties/float_property.h>
#include <rviz/properties/ros_topic_property.h>
#include <rviz/properties/string_property.h>
#include <sensor_msgs/PointCloud2.h>
#include <tf/tf.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <QTime>
#include <functional>
#include <limits>
#include <memory>
#include <stack>
#include "annotation_marker.h"
#include "file_dialog_property.h"
#include "shortcut_property.h"
#include <map>

#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>

#define MKDIR(path)                     \
    do{                                 \
        QDir tempDir;                   \
        if (!tempDir.exists(path))      \
            tempDir.mkpath(path);       \
    }while(0)
#define WRITE_FILE(file_path, data) \
    do{                            \
        QFile fp;                  \
        fp.setFileName(file_path); \
        fp.open(QIODevice::ReadWrite | QIODevice::Text); \
        fp.write(data);            \
        fp.close();                      \
    }while(0)


#define WRITE_JSON_FILE(file_path, doc) \
    do{                            \
        QFile fp;                  \
        fp.setFileName(file_path); \
        fp.open(QIODevice::WriteOnly);   \
        QByteArray data=doc.toJson();    \
        fp.write(data);            \
        fp.close();                      \
    }while(0)
namespace annotate {
    class CloudDisplay;

    class MarkerDisplay;

    class TrackDisplay;

    class AnnotateDisplay : public rviz::DisplayGroup {
    Q_OBJECT
    public:
        AnnotateDisplay();

        void onInitialize() override;

        void setTopic(const QString &topic, const QString &datatype) override;

        void load(const rviz::Config &config) override;

        void setCurrentMarker(AnnotationMarker *marker);

        bool save();

        void publishTrackMarkers();

        sensor_msgs::PointCloud2ConstPtr cloud() const;

        tf::TransformListener &transformListener();

        bool shrinkAfterResize() const;

        bool shrinkBeforeCommit() const;

        bool autoFitAfterPointsChange() const;

        QStringList toolShortcuts() const;

    private Q_SLOTS:

        void updateTopic();

        void updateLabels();

        void updateTags();

        void openFile();

        void updateAnnotationFile();

        void updatePadding();

        void updateMargin();

        void updateIgnoreGround();

        void shrinkToPoints();

        void autoFitPoints();

        void undo();

        void commit();

        void rotateClockwise();

        void rotateAntiClockwise();

        void togglePlayPause();

        void updateShortcuts();

        /*
         * added by oyjy
         */
        void updateLabelNum();

        void updateTagNum();

        void updateRoot();

    private:
        enum PlaybackCommand {
            Play,
            Pause,
            Toggle
        };

        template<class T>
        void modifyChild(rviz::Property *parent, QString const &name, std::function<void(T *)> modifier);

        void adjustView();

        bool load(std::string const &file);

        void createNewAnnotation(const geometry_msgs::PointStamped::ConstPtr &message);

        void handlePointcloud(const sensor_msgs::PointCloud2ConstPtr &cloud);

        void sendPlaybackCommand(PlaybackCommand command);

        ros::NodeHandle node_handle_;
        ros::Subscriber new_annotation_subscriber_;
        ros::Subscriber pointcloud_subscriber_;
        ros::Publisher track_marker_publisher_;
        std::shared_ptr<interactive_markers::InteractiveMarkerServer> server_;
        size_t current_marker_id_{0};
        std::vector<AnnotationMarker::Ptr> markers_;
        std::vector<std::string> labels_;
        std::vector<std::string> tags_;
        std::string filename_;
        ros::Time time_;
        ros::Time last_track_publish_time_;
        sensor_msgs::PointCloud2ConstPtr cloud_;
        tf::TransformListener transform_listener_;
        bool ignore_ground_{false};
        rviz::RosTopicProperty *topic_property_{nullptr};
        rviz::BoolProperty *ignore_ground_property_{nullptr};
        rviz::StringProperty *labels_property_{nullptr};
        rviz::StringProperty *tags_property_{nullptr};
        FileDialogProperty *open_file_property_{nullptr};
        FileDialogProperty *annotation_file_property_{nullptr};
        rviz::Display *cloud_display_{nullptr};
        rviz::Display *marker_display_{nullptr};
        rviz::Display *track_display_{nullptr};
        AnnotationMarker *current_marker_{nullptr};
        rviz::BoolProperty *shortcuts_property_{nullptr};
        ros::ServiceClient playback_client_;
        rviz::BoolProperty *shrink_after_resize_{nullptr};
        rviz::BoolProperty *shrink_before_commit_{nullptr};
        rviz::BoolProperty *auto_fit_after_points_change_{nullptr};
        rviz::BoolProperty *play_after_commit_{nullptr};
        rviz::BoolProperty *pause_after_data_change_{nullptr};
        rviz::FloatProperty *padding_property_{nullptr};
        rviz::FloatProperty *margin_property_{nullptr};

        //added by oyjy
    public:
        rviz::StringProperty *label_num_property_{nullptr};
        rviz::StringProperty *tag_num_property_{nullptr};
        rviz::StringProperty *root_property_{nullptr};

        std::map<std::string, uint32_t> num_labels_;
        std::map<std::string, uint32_t> num_tags_;
        QString dataset_root_path_;
        QString category_file_path;
        QString split_dir_path;

        void setCurrentCount(uint32_t _id) { current_marker_id_ = _id; };
    };

}  // namespace annotate
