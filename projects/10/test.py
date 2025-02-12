import xml.etree.ElementTree as ET

def compare_elements(elem1, elem2):
    # 比较标签
    if elem1.tag != elem2.tag:
        return False

    # 比较属性
    if elem1.attrib != elem2.attrib:
        return False

    # 比较文本内容
    if elem1.text.strip() != elem2.text.strip():
        return False

    # 比较子元素数量
    if len(elem1) != len(elem2):
        return False

    # 递归比较子元素
    for child1, child2 in zip(elem1, elem2):
        if not compare_elements(child1, child2):
            return False

    return True

def compare_xml(file1, file2):
    tree1 = ET.parse(file1)
    tree2 = ET.parse(file2)

    root1 = tree1.getroot()
    root2 = tree2.getroot()

    if compare_elements(root1, root2):
        print("XML files are identical")
    else:
        print("XML files are different")

compare_xml('SquareGame.xml', './Square/SquareGame.xml')