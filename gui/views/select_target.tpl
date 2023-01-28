% include('header.tpl')

<center>
    <form action="/confirm_target" method="post">

        <div id="div_target_name_selection">
            <label for="target_name">Type the target catalogue name (i.e. "M42", "C5" e.t.c):</label><br>
            <input type="text" id="target_name" name="target_name"><br>
        </div>

        <input type="submit" value="Find target coordinates" />
    </form>

    <p  style="padding-top: 3rem">

    <a href="/">
        <input type="button" value="Go back" />
    </a>
</center>

% include('footer.tpl')