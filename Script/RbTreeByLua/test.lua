#!/opt/lua/bin/lua
local m = require "RBTreeModule"
rbTree = m.RBTree:new()
--math.randomseed(os.time())
nodes = {}
for i = 1, 20 do
	nodes[i] = math.random(100)
	rbTree:insert(nodes[i])
end
io.write("插入以下值构建树:")
for i in pairs(nodes) do
	io.write(nodes[i].." ")
end
io.write("\n")

print("按顺序打印所有节点值")
rbTree:printKeyByOrder(rbTree.root)

print("按顺序打印所有节点")
rbTree:printByOrder(rbTree.root)

print("值为40的后继节点")
node = rbTree:search(40)
if node then
	rbTree:nextNode(node):printNode()
else
	print("none")
end

print("删除34，打印所有节点")
rbTree:delete(34)
rbTree:printByOrder(rbTree.root)