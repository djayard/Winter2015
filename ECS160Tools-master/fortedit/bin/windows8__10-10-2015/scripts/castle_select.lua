--local math = require "math"
getCastleLocation = function()
  math.randomseed(os.time())
  number = math.random(0, test.numberCastleLocations - 1)
  return(number)
end
