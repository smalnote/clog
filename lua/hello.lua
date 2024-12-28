-- hello.lua
local count = 0
for _ = 1, 1000000000 do
	count = count + 1
end
print("Hello from lua: ", count)
