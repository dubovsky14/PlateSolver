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
        <p  style="padding-top: 6rem"></p>
        <input type="submit" value="Plate Solve" />


    </form>
</center>



% include('footer.tpl')