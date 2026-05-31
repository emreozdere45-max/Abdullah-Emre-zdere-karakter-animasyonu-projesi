-- N8RO Human Motion assignment runner.
-- Lua owns translation/navigation only; the C++ plugin owns joint angles.

local animationCode = "Idle Shake"
local runSpeedMps = 4.0
local retargetIntervalSeconds = 7.0
local metersPerDegreeLatitude = 111320.0
local degToRad = 0.017453292519943295

local entityState = {}

local function getForwardTarget(entityId)
    if navigation == nil or navigation.getPositionLatLonAlt == nil then
        return nil, nil, nil
    end

    local lat, lon, alt = navigation.getPositionLatLonAlt(entityId)
    if lat == nil or lon == nil or alt == nil then
        return nil, nil, nil
    end

    local headingDeg = 0.0
    if entity ~= nil and entity.getOrientation ~= nil then
        local heading = entity.getOrientation(entityId)
        if heading ~= nil then
            headingDeg = heading
        end
    end

    local distanceMeters = runSpeedMps * retargetIntervalSeconds
    local headingRad = headingDeg * degToRad
    local northMeters = math.cos(headingRad) * distanceMeters
    local eastMeters = math.sin(headingRad) * distanceMeters
    local metersPerDegreeLongitude = metersPerDegreeLatitude * math.cos(lat * degToRad)
    if math.abs(metersPerDegreeLongitude) < 1.0 then
        metersPerDegreeLongitude = metersPerDegreeLatitude
    end

    return lat + (northMeters / metersPerDegreeLatitude),
           lon + (eastMeters / metersPerDegreeLongitude),
           alt
end

local function requestRun(entityId)
    if animation ~= nil and animation.setAnimation ~= nil then
        animation.setAnimation(entityId, animationCode, true, 1.0)
    end

    if navigation == nil or navigation.requestGoTo == nil then
        return false
    end

    local targetLat, targetLon, targetAlt = getForwardTarget(entityId)
    if targetLat == nil then
        return false
    end

    return navigation.requestGoTo(entityId, targetLat, targetLon, targetAlt, runSpeedMps)
end

function onInit(entityId)
    entityState[entityId] = {
        nextRetargetTimeSeconds = retargetIntervalSeconds
    }
    requestRun(entityId)
end

function onTick(entityId, simulationTimeSeconds, deltaTimeSeconds)
    local state = entityState[entityId]
    if state == nil then
        state = {
            nextRetargetTimeSeconds = retargetIntervalSeconds
        }
        entityState[entityId] = state
        requestRun(entityId)
    end

    if simulationTimeSeconds < state.nextRetargetTimeSeconds then
        return
    end

    requestRun(entityId)
    while simulationTimeSeconds >= state.nextRetargetTimeSeconds do
        state.nextRetargetTimeSeconds = state.nextRetargetTimeSeconds + retargetIntervalSeconds
    end
end

function onShutdown(entityId)
    entityState[entityId] = nil
end
