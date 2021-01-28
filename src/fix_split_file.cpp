//
// Created by ou on 2021/1/26.
//
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#define CONFIG_FILE "/home/ou/workspace/ros_ws/ironworks_ws/src/tools/my_annotate/config/annodate.yaml"
using namespace std;

int main() {
    YAML::Node node;
    node = YAML::LoadFile(CONFIG_FILE);
    QString root_path = node["dataset_root"].as<std::string>().c_str();

    vector<string> labels, tags;
    vector<int> labels_num, tags_num;

    //获取当前进度与标签
    YAML::Node node_labels = node["labels"];
    for (size_t i = 0; i < node_labels.size(); ++i) {
        auto it = node_labels[i].begin();
        labels.push_back(it->first.as<string>());
        labels_num.push_back(it->second.as<uint32_t>());
    }

    YAML::Node node_tags = node["tags"];
    for (size_t i = 0; i < node_tags.size(); ++i) {
        auto it = node_tags[i].begin();
        tags.push_back(it->first.as<string>());
        tags_num.push_back(it->second.as<uint32_t>());
    }


    vector<string> cate_dir(labels.size());
    for (int i = 0; i < labels.size(); ++i) {
        QFile fp;
        fp.setFileName(root_path + "/synsetoffset2category.txt");
        fp.open(QIODevice::ReadWrite | QIODevice::Text);
        stringstream ss(fp.readAll().toStdString());
        for (int j = 0; j < labels.size(); ++j) {
            string temp1, temp2;
            ss >> temp1 >> temp2;
            if (labels[i] == temp1) {
                cate_dir[i] = temp2;
                break;
            }
        }
        fp.close();
    }


    QStringList files[labels.size()];
    QDir temp;
    temp.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries); //否则空文件夹也有2个文件
    for (int i = 0; i < labels.size(); i++) {
        temp.setPath(root_path + "/" + cate_dir[i].c_str() + "/points");
        auto lists = temp.entryList();
        files[i] = lists;
        cout << lists.size() << " files in dir <" << temp.path().toStdString() << ">." << endl;
    }

    QJsonArray content;

    for (int i = 0; i < labels.size(); ++i) {
        if (!files[i].empty()) {
            string data = "shape_data/" + cate_dir[i] + "/";
            auto fp = files[i];
            for (int j = 0; j < files[i].size(); ++j) {
                string name = data + fp[j].toStdString();
                content.append(name.c_str());
            }
        }
    }
    QJsonDocument doc(content);
    QFile split;
    split.setFileName(root_path + "/" + "train_test_split/shuffled_train_file_list.json");
    split.open(QIODevice::WriteOnly);
    QByteArray data = doc.toJson();
    split.write(data);
    split.close();
//
//    string split_file_path = root_path + "/" + "train_test_split" + "/shuffled_" + "train" + "_file_list.json";
//    QString path = split_file_path.c_str();

    return 0;
}


