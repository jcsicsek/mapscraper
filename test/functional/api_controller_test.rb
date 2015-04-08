require 'test_helper'

class ApiControllerTest < ActionController::TestCase
  test "should get get_buildings" do
    get :get_buildings
    assert_response :success
  end

end
