# Command line Html Editor
本项目实现一个基于命令行的Html编辑器。

它是本人课程实践，旨在练习基本设计模式（Visitor，Command,composition等）的代码实现。

## 基本功能
该编辑器支持：
- 基本编辑命令
- undo/redo
- 多文件编辑
- 状态保存（类似退出重进VScode时，打开的文件会保留）
- 拼写检查（基于hunspell）

目前支持的命令如下：
### 编辑类命令

#### 1. insert 在某元素之前插入新元素

```bash
insert tagName idValue location [textContent]
```

- tagName 为新元素的元素标签。
- idValue 为新元素的 id，注意 id 不能与其他元素重复。
- insertLocation 为插入位置元素的 id，新元素将插入到该元素之前。
- textContent 为可选参数，表示新元素中的文本内容。

#### 2. append 在某元素内插入新元素

```bash
append tagName idValue parentElement [textContent]
```

- tagName 为新元素的元素标签。
- idValue 为新元素的 id，注意 id 不能与其他元素重复。
- parentElement 为目标父元素的 id，新元素将被插入到该元素内部，并且成为该元素的最后一个子元素。
- textContent 为可选参数，表示新元素中的文本内容。

#### 3. edit-id 编辑某元素的 id

```bash
edit-id oldId newId
```

- oldId 为现有元素的 id。
- newId 为新 id，注意 id 不能与其他元素重复。

#### 4. edit-text 编辑某元素内部的文本

```bash
edit-text element [newTextContent]
```

- element 为要编辑元素的 id。
- newTextContent 为新的文本内容，可以为空，表示清空该元素的文本内容。

#### 5. delete 删除某元素

```bash
delete element
```

- element 为要删除元素的 id。

### 显示类命令

#### 6. print-tree 按树型格式显示

```
print-tree
```
- 有拼写错误的节点会被标上`[X]`
- 默认会显示每个节点的id，可以通过`showid false`调整

#### 7. spell-check 显示拼写检查结果

```
spell-check
```

- 调用拼写检查服务，显示拼写检查结果。

### 输入/输出命令

#### 8. read 读入 HTML 文件

```bash
load filepath
```
- 打开新文件。
- 如果文件不存在，会创建一个文件。

#### 9. save 写入 HTML 文件

```bash
save [filePath]
```

- 未添加filePath参数时，保存当前文件
- 添加了filePath参数时，将当前文件另存为filePath所示的文件

#### 10. 关闭当前文件

```bash
close
```

#### 11. 切换当前文件

``` bash
edit filename
```
- 将当前文件切换为filename所指的的文件。
- 要切换的文件需要已经通过load打开过。

#### 12. 显示文件列表

```bash
edit-list
```

- 显示目前打开的所有文件。
- 当前文件在文件名前面通过'>'标记。
- 有未保存的修改的文件在文件名后面通过'*'标记。

#### 13. 初始化

```bash
init
```

- 将当前文件初始化为一个空的 HTML 模板供后续的编辑。
- 参考模板：
  ```html
  <html>
    <head>
      <title></title>
    </head>
    <body></body>
  </html>
  ```

### 撤销和重做

支持多步撤销与多步重做，规则如下：

- 编辑类指令：支持撤销与重做。如果在撤销后发生了编辑，则此时不允许重做操作。
- 显示类指令：撤销与重做时跳过显示类指令。
- 输入/输出指令：执行输入/输出指令后，不允许撤销与重做。

#### 11. undo 撤销

```bash
undo
```

#### 12. redo 重做

```bash
redo
```

## 运行方式
注意：我全程只在windows系统上测试过代码，我无法保证这个项目在Mac系统上一定可以正常工作。


该项目由纯c++编写。请保证你的编译器支持c++17或以上。

该项目中依赖一些外部库，它们通过vcpkg管理。具体已经在vcpkg.json中注明。如果安装顺利，你将不用管这些外部库是什么。

### 安装vcpkg
你需要拥有vcpkg来运行该项目。假如你已经安装过vcpkg，请直接看下一部分。

进入项目文件夹：
```shell
>cd path/to/HtmlEditor
```

安装vcpkg：
```shell
path/to/HtmlEditor> git clone https://github.com/microsoft/vcpkg.git
path/to/HtmlEditor> cd vcpkg
path/to/HtmlEditor/vcpkg> .\bootstrap-vcpkg.bat 
```
请注意，vcpkg默认使用微软Visual Studio的cl.exe作为编译器。

### 安装依赖
如果你没有vcpkg，或是愿意安装上面的步骤局部配置vcpkg，那么直接执行下面的操作：
```shell
path/to/HtmlEditor/vcpkg> cd ..
path/to/HtmlEditor> .\vcpkg\vcpkg.exe install 
```
此时HtmlEditor下应该会出现一个`vcpkg_installed`目录。之后运行：
```shell
path/to/HtmlEditor> cmake -B build -S .
path/to/HtmlEditor> cmake --build build
```
没有报错说明安装成功。

---

假如你已经有vcpkg，那么，你需要在上面的命令中使用你的vcpkg路径：
```shell
path/to/HtmlEditor> path/to/vcpkg/vcpkg.exe install
```
此时HtmlEditor下应该会出现一个`vcpkg_installed`目录。最后运行：
```shell
path/to/HtmlEditor> cmake -B build -S .
path/to/HtmlEditor> cmake --build build
```

### 运行代码
#### 测试
>！！！不要修改tests目录下的内容

测试代码包括以下几个部分：
- testCmdParser
- test_parser (parser for html file)
- test_spellChecker
- test_node
- test_htmlDoc
- test_printTree
- test_editor

可以通过
```shell
path/to/HtmlEditor> .\build\test\test_xxx.exe
```

分别运行测试。或者通过下面这个批处理文件一起运行：

```shell
path/to/vcpkg> .\run_tests.bat
```
我的测试代码写得比较简约，用了很多assert。中途没有弹窗报错，意味着测试通过。

#### 运行editor

使用HtmlEditor，请运行以下命令，或是直接到对应目录下双击.exe文件：
```shell
path/to/HtmlEditor> .\build\release\HtmlEditor.exe
```
在我的提交文件中，已经包含一个保存的session，它会导致编辑器打开`HtmlEditor/data/foo.html`。如果你不喜欢的话，可以删除data目录下下`config.json`，这样会打开一个空的编辑器。
