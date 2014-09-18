module (..., package.seeall)

Node = {key = 0, left = nil, right = nil, parent = nil, color = nil}
function Node:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function Node:printNode()
	local locleft = self.left and self.left.key or "nil"
	local locright = self.right and self.right.key or "nil"
	local locparent = self.parent and self.parent.key or "nil"
	io.write("{key="..self.key.." left="..locleft.." right="..locright.." parent="..locparent.." color="..self.color.."}\n")
end

RBTree = {root = nil, keysByOrder = {}}
function RBTree:new(o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function RBTree:getKeyByOrder(root)
	if root ~= nil then
		self:getKeyByOrder(root.left)
		table.insert(self.keysByOrder, root.key.." ")
		self:getKeyByOrder(root.right)
	end
end

function RBTree:printKeyByOrder(root)
	self:getKeyByOrder(root)
	for i in pairs(self.keysByOrder) do
		io.write(self.keysByOrder[i].." ")
	end
	io.write("\n")
end

function RBTree:printByOrder(root)
	if root ~= nil then
		self:printByOrder(root.left)
		root:printNode()
		self:printByOrder(root.right)
	end
end

function RBTree:insert(key)
	if self.root == nil then
		self.root = Node:new({key = key, color = 'b'})
		return
	end
	local curNode, lastNode = self.root, nil
	while curNode ~= nil do
		lastNode = curNode
		if key < curNode.key then
			curNode = curNode.left
		else
			curNode = curNode.right
		end
	end
	node = Node:new({key = key, parent = lastNode})
	if key < lastNode.key then
		lastNode.left = node
	else
		lastNode.right = node
	end
	node.color = 'r'
	self:insertAdjust(node)
end

function RBTree:search(key)
	local curNode, lastNode = self.root, nil
	while curNode ~= nil do
		lastNode = curNode
		if key == curNode.key then
			return curNode
		elseif key < curNode.key then
			curNode = curNode.left
		else
			curNode = curNode.right
		end
	end
	return false
end

function RBTree:nextNode(node)
	if node.right == nil then
		while node.parent.key <= node.key and node ~= nil do
			node = node.parent
		end
		return node.parent
	else
		local tmpNode, lastNode = node.right, nil
		while tmpNode ~= nil do
			lastNode = tmpNode
			tmpNode = tmpNode.left
		end
		return lastNode
	end
end

function RBTree:delete(key)
	local delNode, altNode, repNode, nilNode
	delNode = self:search(key)
	if not delNode then
		return false
	end
	if delNode.left == nil or delNode.left == nil then
		altNode = delNode
	else
		altNode = self:nextNode(delNode)
	end
	if altNode.left ~= nil then
		repNode = altNode.left
	else
		repNode = altNode.right
		if not repNode then
			nilNode = Node:new({color = 'b', parent = altNode, key = 0})
			repNode = nilNode
			altNode.right = repNode
		end
	end
	if repNode ~= nil then
		repNode.parent = altNode.parent
	end
	if altNode.parent == nil then
		slef.root = repNode
	else
		if altNode == altNode.parent.left then
			altNode.parent.left = repNode
		else
			altNode.parent.right = repNode
		end
	end
	if altNode ~= delNode then
		delNode.key = altNode.key
	end
	if altNode.color == 'b' then
		self:deleteAdjust(repNode)
	end
	return altNode
end

function RBTree:leftRotate(node)
	local preParent, preRight = node, node.right
	if preRight == nil then
		return false
	end
	preParent.right = preRight.left
	if preRight.left ~= nil then
		preRight.left.parent = preParent
	end
	if preParent ~= self.root then
		if preParent == preParent.parent.left then
			preParent.parent.left = preRight
		else
			preParent.parent.right = preRight
		end
		preRight.parent = preParent.parent
	else
		self.root = preRight
		preRight.parent = nil
	end
	preParent.parent = preRight
	preRight.left = preParent
	return true
end

function RBTree:rightRotate(node)
	local preParent, preLeft = node, node.left
	if preLeft == nil then
		return false
	end
	preParent.left = preLeft.right
	if preLeft.right ~= nil then
		preLeft.right.parent = preParent
	end
	if preParent ~= self.root then
		if preParent == preParent.parent.left then
			preParent.parent.left = preLeft
		else
			preParent.parent.right = preLeft
		end
		preLeft.parent = preParent.parent
	else
		self.root = preLeft
		preLeft.parent = nil
	end
	preParent.parent = preLeft
	preLeft.right = preParent
	return true
end

function RBTree:insertAdjust(node)
	while node.parent.color == 'r' do
		local parentNode = node.parent
		local grandNode = node.parent.parent
		if parentNode == grandNode.left then
			if grandNode.right == nil or grandNode.right.color == 'b' then
				if node == parentNode.right then
					self:leftRotate(parentNode)
				end
				node.color = 'b'
				grandNode.color = 'r'
				self:rightRotate(grandNode)
				break
			else
				parentNode.color = 'b'
				grandNode.color = 'r'
				grandNode.right.color = 'b'
				node = grandNode
				if node == self.root then
					break
				end
			end
		else
			if grandNode.left == nil or grandNode.left.color == 'b' then
				if node == parentNode.left then
					self:rightRotate(parentNode)
				end
				node.color = 'b'
				grandNode.color = 'r'
				self:leftRotate(grandNode)
				break
			else
				parentNode.color = 'b'
				grandNode.color = 'r'
				grandNode.left.color = 'b'
				node = grandNode
				if node == self.root then
					break
				end
			end
		end
	end
	self.root.color = 'b'
end

function RBTree:deleteAdjust(node)
	local nilNode = nil
	if node.key == 0 then
		nilNode = node
	end
	while node.color == 'b' and node ~= self.root do
		local parentNode = node.parent
		if node == parentNode.left then
			local rightBother = parentNode.right
			if rightBother.color == 'r' then
				parentNode.color = 'r'
				rightBother.color = 'b'
				self:leftRotate(parentNode)
			else
				if (rightBother.left == nil or rightBother.left.color == 'b')
				and (rightBother.right == nil or rightBother.right.color == 'b') then
					rightBother.color = 'r'
					node = parentNode
				else
					if rightBother.right == nil or rightBother.right.color == 'b' then
						rightBother.color = 'r'
						rightBother.left.color = 'b'
						self:rightRotate(rightBother)
						rightBother = rightBother.parent
					end
					rightBother.color = parentNode.color
					parentNode.color = 'b'
					rightBother.right.color = 'b'
					self:leftRotate(parentNode)
					node = self.root
				end
			end
		else
			local leftBother = parentNode.left
			if leftBother.color == 'r' then
				parentNode.color = 'r'
				leftBother.color = 'b'
				righRotate(parentNode)
			else
				if (leftBother.right == nil or leftBother.right.color == 'b')
				and (leftBother.left == nil or leftBother.left.color == 'b') then
					leftBother.color = 'r'
					node = parentNode
				else
					if leftBother.left == nil or leftBother.left.color == 'b' then
						leftBother.color = 'r'
						leftBother.right.color = 'b'
						self:leftRotate(leftBother)
						leftBother = leftBother.parent
					end
					leftBother.color = parentNode.color
					parentNode.color = 'b'
					leftBother.left.color = 'b'
					rightRotate(parentNode)
					node = self.root
				end
			end
		end
	end
	node.color = 'b'
	if nilNode then
		nilNode.parent.right = nil
		nilNode.parent.left = nil
		nilNode.parent = nil
	end
end


