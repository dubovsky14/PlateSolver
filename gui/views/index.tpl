% include('header.tpl')

<center>
    <form action="/upload" method="post" enctype="multipart/form-data">
        Select a photo to plate-solve: <input type="file" name="upload" /> <p  style="padding-top: 2rem"></p>
        <input type="submit" value="Plate Solve" />
    </form>
</center>


% include('footer.tpl')