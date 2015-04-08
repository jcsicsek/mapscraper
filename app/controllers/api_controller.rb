class ApiController < ApplicationController
  def get_buildings
    if (params[:address])
      @address = params[:address].to_str()
      @image_url = root_url + "images/" + URI.escape(@address) + ".jpg"
      scale = 2
      zoom = 19
      width = 640
      height = 640
      destination_file = "engine/work/" + @address + ".jpg"
      map_url = "http://maps.googleapis.com/maps/api/staticmap?zoom=19&scale=2&size=640x640&sensor=false&center=" + URI.escape(@address)
      system 'wget -O"'+ destination_file + '" "' + map_url + '"'
      start_time = Time.now
      command = 'engine/mapscraper "' + destination_file + '"'
      output = `#{command}`
      end_time = Time.now
      @elapsed_time = (end_time - start_time) * 1000
      render :xml => output
    end
  end
  
  def get_map_image
    if (params[:address])
      @address = params[:address].to_str()
      @image_path = "public/images/" + @address + ".jpg"
      scale = 2
      zoom = 19
      width = 640
      height = 640
      destination_file = "engine/work/" + @address + ".jpg"
      map_url = "http://maps.googleapis.com/maps/api/staticmap?zoom=19&scale=2&size=640x640&sensor=false&center=" + URI.escape(@address)
      system 'wget -O"'+ destination_file + '" "' + map_url + '"'
      start_time = Time.now
      command = 'engine/mapscraper "' + destination_file + '"'
      output = `#{command}`
      end_time = Time.now
      @elapsed_time = (end_time - start_time) * 1000
      send_file @image_path, :type => "image/jpeg"
    end
  end
end
