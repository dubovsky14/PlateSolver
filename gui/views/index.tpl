% include('header.tpl')


<center>
    <form action="/upload" method="post" enctype="multipart/form-data">

        <label for="file">Select a photo to plate-solve:</label>
        <input type="file" id="file" name="upload" /> <p  style="padding-top: 2rem"></p>
         <label for="index_file">Choose an index_file:</label>

        <select name="index_file" id="index_file">
        % for index_file_name in index_files:
          <option value="{{index_file_name}}" {{"/selected" if index_file_name == selected_index_file else ""}}>{{index_file_name}}</option>
        % end
        </select>
        <p  style="padding-top: 3rem"></p>
        <div>
            <label>
                %checked = "CHECKED" if annotate_default else ""
                <input type="checkbox" id="checkbox_annotate" name="checkbox_annotate" {{checked}}> Show annotated photo
            </label>
        </div>

        <p  style="padding-top: 3rem"></p>


        <input type="submit" value="Plate Solve" />

        <p  style="padding-top: 3rem"></p>

        <b>Select target coordinates (optional):</b> <br>

        <p  style="padding-top: 1rem"></p>

        <div style="display: table-row;width:100%" horizontal layout id="div_target_coordinates_selection">
            <div style="display:table-cell;width: 50%;text-align:left" id="div_target_ra_selection">
                <label for="target_ra">Right ascension:</label><br>
                <input type="text" id="target_ra" name="target_ra" value="{{target_ra}}"><br>
            </div>
            <div style="display:table-cell;width: 50%;text-align:right" id="div_target_dec_selection">
                <label for="target_dec">Declination:</label><br>
                <input type="text" id="target_dec" name="target_dec" value="{{target_dec}}"><br>
            </div>
        </div>

    </form>
</center>



% include('footer.tpl')