
function GetFileLoader(filename) 
  local File = ASE.Java["java.io.File"]
  local URL = ASE.Java["java.net.URL"]
  local URLClassLoader = ASE.Java["java.net.URLClassLoader"]
  local burl = File(filename).getCanonicalFile().toURL()
  local urls = URL.NewArrayValue(burl)
  return URLClassLoader(urls)
end

