# coding=utf-8
import numpy as np
import yaml
import json
import os


def create_file(filename):
    path = filename[0:filename.rfind("/")]
    if not os.path.isdir(path):  # 无文件夹时创建
        os.makedirs(path)
    if not os.path.isfile(filename):  # 无文件时创建
        fd = open(filename, mode="w")
        fd.close()
    else:
        pass


def main(config_file_path):
    category_file_name = "synsetoffset2category.txt"
    train_test_split = 1.0 / 4.0  # 防止python2变成0
    train_split_file_name = "shuffled_train_file_list.json"
    test_split_file_name = "shuffled_test_file_list.json"

    with open(config_file_path, 'r') as fs:
        datas = yaml.load(fs)

    # 获取各个类的路径,存入label_dir
    dataset_root_path = datas["dataset_root"]
    with open(os.path.join(dataset_root_path, category_file_name), 'r') as fs:
        labels = fs.readlines()
        labels_dir = [label.strip().split('\t') for label in labels]

    # 获取每个类别路径下.../points/的数据文件名
    files, num, train_data, test_data = [], [], [], []
    for label_dir, i in zip(labels_dir, range(len(labels_dir))):
        files.append(os.listdir(os.path.join(dataset_root_path, label_dir[1], "points")))
        np.random.shuffle(files[i])
        datas["labels"][label_dir[0]] = len(files[i])
        for it in range(len(files[i])):
            st = "shape_data/" + label_dir[1] + "/" + files[i][it]
            if it < int(len(files[i]) * train_test_split):
                test_data.append(st)
            else:
                train_data.append(st)

    datas["tags"] = {}
    for type in ["test", "train"]:
        path = os.path.join(dataset_root_path, "train_test_split/" + locals()[type + "_split_file_name"])
        create_file(path)
        with open(path, 'w') as f:
            data = locals()[type + "_data"]
            np.random.shuffle(data)
            json.dump(data, f)
        # datas["tags"].append({type: len(locals()[type + "_data"])})
        datas["tags"][type] = len(locals()[type + "_data"])

    with open(config_file_path, 'w') as fs:
        yaml.dump(datas, fs, default_flow_style=False)


config_file_path = "/home/ou/workspace/ros_ws/ironworks_ws/src/tools/my_annotate/config/annodate.yaml"
main(config_file_path)
# dataset_root: /home/ou/Documents/dataset/my_dataset
# labels: {car: 206, ironpile: 149, pedestrian: 154, tree: 100}
# tags: {test: 151, train: 458}
