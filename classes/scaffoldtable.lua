mrequire "classes/class"

newclass("ScaffoldTable",
    function(callback, getCallback, lastSeparate, ...)
        lastSeparate = lastSeparate or false
    
        return { 
            __callback = callback,
            __getCallback = getCallback,
            __lastSeparate = lastSeparate,
            __extra = { ... }
        }
    end
)

function ScaffoldTable:getBreadcrumbs()
    local parent = rawget(self, "__parent")
    if parent then
        local crumbs, root = parent:getBreadcrumbs()
        table.insert(crumbs, rawget(self, "__key"))
        return crumbs, root
    end
    
    return { }, self
end

function ScaffoldTable:__index(key)
    return rawget(ScaffoldTable, key) 
        or setmetatable({ __parent = self, __key = key }, ScaffoldTable)
end

function ScaffoldTable:__newindex(key, value)
    local crumbs, root = self:getBreadcrumbs()
    local callback = rawget(root, "__callback")
    local extra = rawget(root, "__extra")
    
    if rawget(root, "__lastSeparate") then
        callback(crumbs, key, value, unpack(extra))
    else
        table.insert(crumbs, key)
        callback(crumbs, value, unpack(extra))
    end
end

function ScaffoldTable:__assign(value)
    local crumbs, root = self:getBreadcrumbs()
    local callback = rawget(root, "__callback")
    local extra = rawget(root, "__extra")
    
    callback(crumbs, value, unpack(extra))
end

function ScaffoldTable:__deref()
    local crumbs, root = self:getBreadcrumbs()
    local callback = rawget(root, "__getCallback")
    local extra = rawget(root, "__extra")
    
    if callback then
        return callback(crumbs, value, unpack(extra))
    else
        error("No dereference callback was assigned to the ScaffoldTable")
    end
end
